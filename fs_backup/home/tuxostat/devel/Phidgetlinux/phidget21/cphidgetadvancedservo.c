#include "stdafx.h"
#include "cphidgetadvancedservo.h"
#include "cusb.h"
#include "csocket.h"
#include "cthread.h"

// === Internal Functions === //
static int CPhidgetAdvancedServo_makePacket(CPhidgetAdvancedServoHandle phid, unsigned char *buffer, int Index);

//clearVars - sets all device variables to unknown state
CPHIDGETCLEARVARS(AdvancedServo)
	int i = 0;

	phid->motorPositionMaxLimit = PUNK_DBL;
	phid->motorPositionMinLimit = PUNK_DBL;
	phid->velocityMax = PUNK_DBL;
	phid->velocityMin = PUNK_DBL;
	phid->accelerationMax = PUNK_DBL;
	phid->accelerationMin = PUNK_DBL;

	for (i = 0; i<ADVSERVO_MAXSERVOS; i++)
	{
		phid->motorVelocityEcho[i] = PUNK_DBL;
		phid->motorPositionEcho[i] = PUNK_DBL;
		phid->motorSensedCurrent[i] = PUNK_DBL;
		phid->motorSpeedRampingStateEcho[i] = PUNK_BOOL;
		phid->motorStoppedState[i] = PUNK_BOOL;
		phid->motorEngagedStateEcho[i] = PUNK_BOOL;
		phid->packetCounterEcho[i] = PUNK_INT;
		
		phid->motorPosition[i] = PUNK_DBL;
		phid->motorAcceleration[i] = PUNK_DBL;
		phid->motorVelocity[i] = PUNK_DBL;
		phid->motorSpeedRampingState[i] = PUNK_BOOL;
		phid->motorEngagedState[i] = PUNK_BOOL;
		phid->packetCounter[i] = PUNK_INT;

		phid->motorPositionMax[i] = PUNK_DBL;
		phid->motorPositionMin[i] = PUNK_DBL;
	}
	return EPHIDGET_OK;
}

//initAfterOpen - sets up the initial state of an object, reading in packets from the device if needed
//				  used during attach initialization - on every attach
CPHIDGETINIT(AdvancedServo)
	int i = 0, result = 0;
	unsigned char buffer[8];

	TESTPTR(phid);
	ZEROMEM(buffer, 8);

	//Setup max/min values
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_ADVANCEDSERVO8:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				phid->motorPositionMaxLimit = -23 + 256;	//233
				phid->motorPositionMinLimit = -23 + (1/128.0); //-22.9921875
				phid->velocityMax = (50/128.0) * 16384; //6400
				phid->velocityMin = 0;
				phid->accelerationMax = (50/128.0) / 0.02 * 16384; //320000
				phid->accelerationMin = (50/128.0) / 0.02; //min velocity over time integration increment - 19.53125
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		default:
			return EPHIDGET_UNEXPECTED;
	}

	//initialize triggers, set data arrays to unknown
	for (i = 0; i<phid->phid.attr.advancedservo.numMotors; i++)
	{
		phid->motorVelocityEcho[i] = PUNK_DBL;
		phid->motorPositionEcho[i] = PUNK_DBL;
		phid->motorSensedCurrent[i] = PUNK_DBL;
		phid->motorSpeedRampingStateEcho[i] = PUNK_BOOL;
		phid->motorStoppedState[i] = PUNK_BOOL;
		phid->motorEngagedStateEcho[i] = PUNK_BOOL;
		phid->packetCounterEcho[i] = PUNK_INT;
		phid->packetCounter[i] = PUNK_INT;
	}

	//read in initial state - only one packet needed
	CPhidget_read((CPhidgetHandle)phid);

	//At this point, we can only recover (maybe) the position, speed ramping, engaged state.
	for (i = 0; i<phid->phid.attr.advancedservo.numMotors; i++)
	{
		phid->motorPosition[i] = phid->motorPositionEcho[i];
		phid->motorAcceleration[i] = PUNK_DBL;
		phid->motorVelocity[i] = PUNK_DBL;
		phid->motorSpeedRampingState[i] = phid->motorSpeedRampingStateEcho[i];
		phid->motorEngagedState[i] = phid->motorEngagedStateEcho[i];
		phid->packetCounter[i] = phid->packetCounterEcho[i];

		if(phid->motorStoppedState[i] == PUNK_BOOL)
		{
			if(phid->motorVelocityEcho[i] == 0 || phid->motorEngagedStateEcho[i] == PFALSE)
				phid->motorStoppedState[i] = PTRUE;
			else
				phid->motorStoppedState[i] = PFALSE;
		}
	}

	//we need to set the min/max positions for each motor because these value cannot be read back and we don't want them to be undefined
	for (i = 0; i<phid->phid.attr.advancedservo.numMotors; i++)
	{
		phid->motorPositionMax[i] = phid->motorPositionMaxLimit;
		phid->motorPositionMin[i] = phid->motorPositionMinLimit;
		if((result = CPhidgetAdvancedServo_makePacket(phid, buffer, i + ADVSERVO_MINMAX_PACKET)))
			return result;
		if ((result = CUSBSendPacket((CPhidgetHandle)phid, buffer)))
			return result;
	}

	return EPHIDGET_OK;
}

//dataInput - parses device packets
CPHIDGETDATA(AdvancedServo)
	int i=0;

	unsigned char speedRamping[ADVSERVO_MAXSERVOS], motorEngaged[ADVSERVO_MAXSERVOS], motorDone[ADVSERVO_MAXSERVOS], justStopped[ADVSERVO_MAXSERVOS];
	double velocity[ADVSERVO_MAXSERVOS], position[ADVSERVO_MAXSERVOS], current[ADVSERVO_MAXSERVOS];
	double lastVelocity[ADVSERVO_MAXSERVOS], lastPosition[ADVSERVO_MAXSERVOS], lastCurrent[ADVSERVO_MAXSERVOS];
	int pwmEcho[ADVSERVO_MAXSERVOS];

	if (length < 0) return EPHIDGET_INVALIDARG;
	TESTPTR(phid);
	TESTPTR(buffer);

	//Parse device packets - store data locally
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_ADVANCEDSERVO8:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				for (i = 0; i < phid->phid.attr.advancedservo.numMotors; i++)
				{
					phid->packetCounterEcho[i] = buffer[0+(i*7)] & 0x0F;
					motorEngaged[i] = (buffer[0+(i*7)] & MOTOR_DISABLED_ADVSERVO) ? PFALSE : PTRUE;
					speedRamping[i] = (buffer[0+(i*7)] & NO_RAMPING_FLAG_ADVSERVO) ? PFALSE : PTRUE;
					motorDone[i] = (buffer[0+(i*7)] & MOTOR_DONE_ADVSERVO) ? PTRUE : PFALSE;

					pwmEcho[i] = (unsigned short)(buffer[1+(i*7)] << 8) + (unsigned char)(buffer[2+(i*7)]);
					position[i] = round_double(((pwmEcho[i]/128.0) - 23), 3);

					velocity[i] = (signed short)(buffer[3+(i*7)] << 8) + (unsigned char)(buffer[4+(i*7)]);
					velocity[i] = round_double(((velocity[i]/16384.0)*phid->velocityMax), 2);

					current[i] = (unsigned short)(buffer[5+(i*7)] << 8) + (unsigned char)(buffer[6+(i*7)]);
					current[i] = round_double(((50.0/11.0) * (current[i]/16384.0)), 4);
				}
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		default:
			return EPHIDGET_UNEXPECTED;
	}

	//Make sure values are within defined range, and store to structure
	for (i = 0; i < phid->phid.attr.advancedservo.numMotors; i++)
	{
		lastPosition[i] = phid->motorPositionEcho[i];
		lastVelocity[i] = phid->motorVelocityEcho[i];
		lastCurrent[i] = phid->motorSensedCurrent[i];

		if(position[i] > phid->motorPositionMaxLimit || position[i] < phid->motorPositionMinLimit)
			phid->motorPositionEcho[i] = PUNK_DBL;
		else
			phid->motorPositionEcho[i] = position[i];
		if(velocity[i] > phid->velocityMax || velocity[i] < -phid->velocityMax)
			LOG(PHIDGET_LOG_WARNING, "Phidget advanced servo recieved out of range velocity data: %lf", velocity[i]);
		else
			phid->motorVelocityEcho[i] = velocity[i];

		phid->motorSensedCurrent[i] = current[i];

		phid->motorSpeedRampingStateEcho[i] = speedRamping[i];
		phid->motorEngagedStateEcho[i] = motorEngaged[i];
	}

	//make sure phid->motorStoppedState isn't updated until the other data is filled in
	CThread_mutex_lock(&phid->phid.writelock);
	for (i = 0; i < phid->phid.attr.stepper.numMotors; i++)
	{
		int pwm = round((phid->motorPosition[i] + 23.0) * 128.0);
		//if we are up to date, and the motor is done, set stopped to true - this is the only place that this gets set true;
		justStopped[i] = PFALSE;
		if(phid->packetCounter[i] == phid->packetCounterEcho[i] && motorDone[i] == PTRUE
			&& ((pwmEcho[i] == pwm && phid->motorVelocityEcho[i] == 0) || phid->motorEngagedStateEcho[i] == PFALSE))
		{
			if(phid->motorStoppedState[i] == PFALSE)
				justStopped[i] = PTRUE;
			phid->motorStoppedState[i] = PTRUE;
		}
		else if(motorDone[i] == PFALSE)
			phid->motorStoppedState[i] = PFALSE;
	}
	CThread_mutex_unlock(&phid->phid.writelock);

	//send out any events for changed data
	//only send a position event if the motor is engaged
	for (i = 0; i < phid->phid.attr.advancedservo.numMotors; i++)
	{
		if(phid->motorPositionEcho[i] != PUNK_DBL && phid->motorEngagedStateEcho[i] == PTRUE
			&& (phid->motorPositionEcho[i] != lastPosition[i] || justStopped[i] == PTRUE) )
			FIRE(PositionChange, i, phid->motorPositionEcho[i]);

		if(phid->motorVelocityEcho[i] != PUNK_DBL && phid->motorVelocityEcho[i] != lastVelocity[i])
			FIRE(VelocityChange, i, phid->motorVelocityEcho[i]);

		if(phid->motorSensedCurrent[i] != PUNK_DBL && phid->motorSensedCurrent[i] != lastCurrent[i])
			FIRE(CurrentChange, i, phid->motorSensedCurrent[i]);
	}
	
	return EPHIDGET_OK;
}

//eventsAfterOpen - sends out an event for all valid data, used during attach initialization
CPHIDGETINITEVENTS(AdvancedServo)
	int i = 0;

	for (i = 0; i<phid->phid.attr.advancedservo.numMotors; i++)
	{
		if(phid->motorSensedCurrent[i] != PUNK_DBL)
			FIRE(CurrentChange, i, phid->motorSensedCurrent[i]);
		if(phid->motorVelocityEcho[i] != PUNK_DBL)
			FIRE(VelocityChange, i, phid->motorVelocityEcho[i]);
		if(phid->motorPositionEcho[i] != PUNK_DBL && phid->motorEngagedStateEcho[i] == PTRUE)
			FIRE(PositionChange, i, phid->motorPositionEcho[i]);
	}

	return EPHIDGET_OK;
}

//getPacket - used by write thread to get the next packet to send to device
CGETPACKET_BUF(AdvancedServo)

//sendpacket - sends a packet to the device asynchronously, blocking if the 1-packet queue is full
CSENDPACKET_BUF(AdvancedServo)

//makePacket - constructs a packet using current device state
CMAKEPACKETINDEXED(AdvancedServo)
	int pwm = 0, velocity = 0, accel = 0, minpwm=0, maxpwm=0;
	unsigned char flags = 0;

	int packet_type = Index & 0x10;
	Index = Index & 0x07;

	TESTPTRS(phid, buffer);

	if(phid->packetCounter[Index] == PUNK_INT)
		phid->packetCounter[Index] = 0;

	phid->packetCounter[Index]++;
	phid->packetCounter[Index] &= 0x0F;
	
	phid->motorStoppedState[Index] = PFALSE;

	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_ADVANCEDSERVO8:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{

				if(phid->motorSpeedRampingState[Index] == PUNK_BOOL)
					phid->motorSpeedRampingState[Index] = PTRUE;
				if(phid->motorEngagedState[Index] == PUNK_BOOL)
					phid->motorEngagedState[Index] = PFALSE; //note motors are not engaged by default

				if(phid->motorSpeedRampingState[Index] == PFALSE)
					flags |= NO_RAMPING_FLAG_ADVSERVO;

				if(phid->motorEngagedState[Index] == PFALSE)
					flags |= MOTOR_DISABLED_ADVSERVO;

				//2-bit index, 2-bit packet type, 4-bit counter 
				buffer[0] = (Index << 5) | packet_type | phid->packetCounter[Index]; 
				buffer[1] = flags;

				switch(packet_type)
				{
					case ADVSERVO_NORMAL_PACKET:
						//have to make sure that everything to be sent has some sort of default value if the user hasn't set a value
						if(phid->motorPosition[Index] == PUNK_DBL)
							phid->motorPosition[Index] = 105; //mid-range valid posn (much better then 0)
						if(phid->motorVelocity[Index] == PUNK_DBL)
							phid->motorVelocity[Index] = 316; //max speed of the servo Phidgets sells (0.19sec/60 degrees at 4.8V) //TODO: could bump this because it's 5V
						if(phid->motorAcceleration[Index] == PUNK_DBL)
							phid->motorAcceleration[Index] = phid->accelerationMax / 2; //mid-level acceleration
						pwm = round((phid->motorPosition[Index] + 23.0) * 128.0);
						velocity = round((phid->motorVelocity[Index] / phid->velocityMax) * 16384);
						accel = round((phid->motorAcceleration[Index] / phid->accelerationMax) * 16384);

						buffer[2] = (unsigned char)((pwm >> 8) & 0xff);
						buffer[3] = (unsigned char)(pwm & 0xff); 
						buffer[4] = (unsigned char)((velocity >> 8) & 0xff);
						buffer[5] = (unsigned char)(velocity & 0xff);
						buffer[6] = (unsigned char)((accel >> 8) & 0xff);
						buffer[7] = (unsigned char)(accel & 0xff);
						break;
					case ADVSERVO_MINMAX_PACKET:
						minpwm = round((phid->motorPositionMin[Index] + 23.0) * 128.0);
						maxpwm = round((phid->motorPositionMax[Index] + 23.0) * 128.0);

						buffer[2] = (unsigned char)((minpwm >> 8) & 0xff);
						buffer[3] = (unsigned char)(minpwm & 0xff); 
						buffer[4] = (unsigned char)((maxpwm >> 8) & 0xff);
						buffer[5] = (unsigned char)(maxpwm & 0xff);
						buffer[6] = 0;
						buffer[7] = 0;
						break;
					default:
						return EPHIDGET_UNEXPECTED;
				}
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		default:
			return EPHIDGET_UNEXPECTED;
	}

	return EPHIDGET_OK;
}

// === Exported Functions === //

//create and initialize a device structure
CCREATE(AdvancedServo, PHIDDEF_ADVANCEDSERVO)

//event setup functions
CFHANDLE(AdvancedServo, PositionChange, int, double)
CFHANDLE(AdvancedServo, VelocityChange, int, double)
CFHANDLE(AdvancedServo, CurrentChange, int, double)

CGET(AdvancedServo,MotorCount,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	MASGN(phid.attr.advancedservo.numMotors);
}

CGETINDEX(AdvancedServo,Acceleration,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTMASGN(motorAcceleration[Index], PUNK_DBL)
	MASGN(motorAcceleration[Index])
}
CSETINDEX(AdvancedServo,Acceleration,double)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTRANGE(phid->accelerationMin, phid->accelerationMax)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(Acceleration, "%lf");
	else
		SENDPACKETINDEXED(AdvancedServo, motorAcceleration[Index], Index + ADVSERVO_NORMAL_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(AdvancedServo,AccelerationMax,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	MASGN(accelerationMax)
}

CGETINDEX(AdvancedServo,AccelerationMin,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	MASGN(accelerationMin)
}

CGETINDEX(AdvancedServo,VelocityLimit,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTMASGN(motorVelocity[Index], PUNK_DBL)
	MASGN(motorVelocity[Index])
}
CSETINDEX(AdvancedServo,VelocityLimit,double)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTRANGE(phid->velocityMin, phid->velocityMax)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(VelocityLimit, "%lf");
	else
		SENDPACKETINDEXED(AdvancedServo, motorVelocity[Index], Index + ADVSERVO_NORMAL_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(AdvancedServo,Velocity,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTMASGN(motorVelocityEcho[Index], PUNK_DBL)
	MASGN(motorVelocityEcho[Index])
}

CGETINDEX(AdvancedServo,VelocityMax,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	MASGN(velocityMax)
}

CGETINDEX(AdvancedServo,VelocityMin,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	MASGN(velocityMin)
}

CGETINDEX(AdvancedServo,Position,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTMASGN(motorPositionEcho[Index], PUNK_DBL)
	//send unknown if the motor is not engaged
	if(phid->motorEngagedStateEcho[Index] != PTRUE) {*pVal = PUNK_DBL; return EPHIDGET_UNKNOWNVAL;}
	MASGN(motorPositionEcho[Index])
}
CSETINDEX(AdvancedServo,Position,double)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTRANGE(phid->motorPositionMin[Index], phid->motorPositionMax[Index])

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(Position, "%lf");
	else
		SENDPACKETINDEXED(AdvancedServo, motorPosition[Index], Index + ADVSERVO_NORMAL_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(AdvancedServo,PositionMax,double)
	TESTPTRS(phid,pVal)
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	MASGN(motorPositionMax[Index])
}
CSETINDEX(AdvancedServo,PositionMax,double)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTRANGE(phid->motorPositionMin[Index], phid->motorPositionMaxLimit)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(PositionMax, "%lf");
	else
		SENDPACKETINDEXED(AdvancedServo, motorPositionMax[Index], Index + ADVSERVO_MINMAX_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(AdvancedServo,PositionMin,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	MASGN(motorPositionMin[Index])
}
CSETINDEX(AdvancedServo,PositionMin,double)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTRANGE(phid->motorPositionMinLimit, phid->motorPositionMax[Index])

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(PositionMin, "%lf");
	else
		SENDPACKETINDEXED(AdvancedServo, motorPositionMin[Index], Index + ADVSERVO_MINMAX_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(AdvancedServo,Current,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTMASGN(motorSensedCurrent[Index], PUNK_DBL)
	MASGN(motorSensedCurrent[Index])
}

CGETINDEX(AdvancedServo,SpeedRampingOn,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTMASGN(motorSpeedRampingStateEcho[Index], PUNK_BOOL)
	MASGN(motorSpeedRampingStateEcho[Index]);
}
CSETINDEX(AdvancedServo,SpeedRampingOn,int)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTRANGE(PFALSE, PTRUE)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(SpeedRampingOn, "%d");
	else
		SENDPACKETINDEXED(AdvancedServo, motorSpeedRampingState[Index], Index + ADVSERVO_NORMAL_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(AdvancedServo,Engaged,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTMASGN(motorEngagedStateEcho[Index], PUNK_BOOL)
	MASGN(motorEngagedStateEcho[Index])
}
CSETINDEX(AdvancedServo,Engaged,int)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTRANGE(PFALSE, PTRUE)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(Engaged, "%d");
	else
		SENDPACKETINDEXED(AdvancedServo, motorEngagedState[Index], Index + ADVSERVO_NORMAL_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(AdvancedServo,Stopped,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ADVANCEDSERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.advancedservo.numMotors)
	TESTMASGN(motorStoppedState[Index], PUNK_BOOL)
	MASGN(motorStoppedState[Index])
}
