#include "stdafx.h"
#include "cphidgetstepper.h"
#include "cusb.h"
#include "csocket.h"
#include "cthread.h"

// === Internal Functions === //

//clearVars - sets all device variables to unknown state
CPHIDGETCLEARVARS(Stepper)
	int i = 0;

	phid->motorSpeedMax = PUNK_DBL;
	phid->motorSpeedMin = PUNK_DBL;
	phid->accelerationMax = PUNK_DBL;
	phid->accelerationMin = PUNK_DBL;
	phid->motorPositionMax = PUNK_INT64;
	phid->motorPositionMin = PUNK_INT64;
	phid->currentMax = PUNK_DBL;
	phid->currentMin = PUNK_DBL;

	for (i = 0; i<STEPPER_MAXINPUTS; i++)
	{
		phid->inputState[i] = PUNK_BOOL;
	}

	for (i = 0; i<STEPPER_MAXSTEPPERS; i++)
	{
		phid->motorSpeedEcho[i] = PUNK_DBL;
		phid->motorPositionEcho[i] = PUNK_INT64;
		phid->motorSensedCurrent[i] = PUNK_DBL;
		phid->motorEngagedStateEcho[i] = PUNK_BOOL;
		phid->motorStoppedState[i] = PUNK_BOOL;
		phid->packetCounterEcho[i] = PUNK_INT;
		
		phid->motorPosition[i] = PUNK_INT64;
		phid->motorPositionReset[i] = PUNK_INT64;
		phid->motorEngagedState[i] = PUNK_BOOL;
		phid->motorAcceleration[i] = PUNK_DBL;
		phid->motorSpeed[i] = PUNK_DBL;
		phid->motorCurrentLimit[i] = PUNK_DBL;
		phid->packetCounter[i] = PUNK_INT;
	}
	return EPHIDGET_OK;
}

//initAfterOpen - sets up the initial state of an object, reading in packets from the device if needed
//				  used during attach initialization - on every attach
CPHIDGETINIT(Stepper)
	int i = 0;

	TESTPTR(phid);
	
	//Setup max/min values
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_UNIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				phid->microSteps = 2;
				phid->motorSpeedMax = ((phid->microSteps * 0x100) - 1) * 0.75;
				phid->motorSpeedMin = 0;
				phid->accelerationMax = (250 * (0.75 * 0.75)) * 0x3f;
				phid->accelerationMin = (250 * (0.75 * 0.75));
				phid->motorPositionMax = 0x7FFFFFFFFFLL;
				phid->motorPositionMin = -0x7FFFFFFFFFLL;
				phid->currentMax = PUNK_DBL;
				phid->currentMin = PUNK_DBL;
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_BIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				phid->microSteps = 16;
				phid->motorSpeedMax = 4 * phid->microSteps * 0x100; //0x4000
				phid->motorSpeedMin = 0;
				phid->accelerationMax = 4000 * 0xff;
				phid->accelerationMin = 4000;
				phid->motorPositionMax = 0x7FFFFFFFFFLL;
				phid->motorPositionMin = -0x7FFFFFFFFFLL;
				phid->currentMax = 2.492;
				phid->currentMin = 0.0542;
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_STEPPER:
		default:
			return EPHIDGET_UNEXPECTED;
	}

	//set data arrays to unknown
	for (i = 0; i<phid->phid.attr.stepper.numInputs; i++)
	{
		phid->inputState[i] = PUNK_BOOL;
	}
	for (i = 0; i<phid->phid.attr.stepper.numMotors; i++)
	{
		phid->motorSpeedEcho[i] = PUNK_DBL;
		phid->motorPositionEcho[i] = PUNK_INT64;
		phid->motorSensedCurrent[i] = PUNK_DBL;
		phid->motorEngagedStateEcho[i] = PUNK_BOOL;
		phid->motorStoppedState[i] = PUNK_BOOL;
		phid->packetCounterEcho[i] = PUNK_INT;
		phid->packetCounter[i] = PUNK_INT;
	}

	//read in initial state - only one packet needed
	CPhidget_read((CPhidgetHandle)phid);

	//At this point, we can only recover (maybe) the position, engaged state, set others to unknown
	for (i = 0; i<phid->phid.attr.advancedservo.numMotors; i++)
	{
		phid->motorPosition[i] = phid->motorPositionEcho[i];
		phid->motorEngagedState[i] = phid->motorEngagedStateEcho[i];
		phid->motorAcceleration[i] = PUNK_DBL;
		phid->motorSpeed[i] = PUNK_DBL;
		phid->motorCurrentLimit[i] = PUNK_DBL;
		phid->packetCounter[i] = phid->packetCounterEcho[i];

		if(phid->motorStoppedState[i] == PUNK_BOOL)
		{
			if(phid->motorSpeedEcho[i] == 0 || phid->motorEngagedStateEcho[i] == PFALSE)
				phid->motorStoppedState[i] = PTRUE;
			else
				phid->motorStoppedState[i] = PFALSE;
		}
	}
	
	return EPHIDGET_OK;
}

//dataInput - parses device packets
CPHIDGETDATA(Stepper)
	int i = 0, j = 0;

	double speed[STEPPER_MAXSTEPPERS], current[STEPPER_MAXSTEPPERS];
	double lastSpeed[STEPPER_MAXSTEPPERS], lastCurrent[STEPPER_MAXSTEPPERS];
	__int64 position[STEPPER_MAXSTEPPERS], lastPosition[STEPPER_MAXSTEPPERS];
	unsigned char input[STEPPER_MAXINPUTS], lastInput[STEPPER_MAXINPUTS];
	unsigned char motorEngaged[STEPPER_MAXSTEPPERS], motorDone[STEPPER_MAXSTEPPERS], justStopped[STEPPER_MAXSTEPPERS];

	if (length < 0) return EPHIDGET_INVALIDARG;
	TESTPTR(phid);
	TESTPTR(buffer);

	ZEROMEM(input, sizeof(input));
	ZEROMEM(lastInput, sizeof(lastInput));

	//Parse device packets - store data locally
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_UNIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				for (i = 0; i < phid->phid.attr.stepper.numMotors; i++)
				{
					phid->packetCounterEcho[i] = buffer[0+(i*9)] & 0x0F;
					motorEngaged[i] = (buffer[0+(i*9)] & MOTOR_DISABLED_STEPPER) ? PFALSE : PTRUE;
					motorDone[i] = (buffer[0+(i*9)] & MOTOR_DONE_STEPPER) ? PTRUE : PFALSE;

					speed[i] = (double)((signed short)((buffer[1+(i*9)] << 8) | buffer[2+(i*9)]));
					speed[i] = (double)((speed[i] / 511.0) * phid->motorSpeedMax);

					position[i] = ((((__int64)(signed char)buffer[3+(i*9)]) << 40) +
						(((__uint64)buffer[4+(i*9)]) << 32) +
						(((__uint64)buffer[5+(i*9)]) << 24) +
						(((__uint64)buffer[6+(i*9)]) << 16) +
						(((__uint64)buffer[7+(i*9)]) << 8) +
						((__uint64)buffer[8+(i*9)]));

					position[i] -= 0x20; //round
					position[i] >>= 6;

					//current is not returned
					current[i] = PUNK_DBL;
				}
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_BIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				phid->packetCounterEcho[0] = buffer[0] & 0x0F;
				motorEngaged[0] = (buffer[0] & MOTOR_DISABLED_STEPPER) ? PFALSE : PTRUE;
				motorDone[0] = (buffer[0] & MOTOR_DONE_STEPPER) ? PTRUE : PFALSE;

				speed[0] = (double)((signed short)((buffer[1] << 8) | buffer[2]));
				speed[0] = (double)((speed[i] / 2048.0) * phid->motorSpeedMax);

				position[0] = ((((__int64)(signed char)buffer[3]) << 40) +
					(((__uint64)buffer[4]) << 32) +
					(((__uint64)buffer[5]) << 24) +
					(((__uint64)buffer[6]) << 16) +
					(((__uint64)buffer[7]) << 8) +
					((__uint64)buffer[8]));

				position[0] -= 0x04; //round
				position[0] >>= 3;

				for (i = 0, j=0x01; i<phid->phid.attr.stepper.numInputs; i++, j<<=1)
				{
					if ((buffer[9] & j))
						input[i] = PFALSE;
					else
						input[i] = PTRUE;
				}

				//value is 0-4.16 v
				current[0] = (((unsigned char)buffer[10] * 4.16 ) / 255.0);
				current[0] = round_double((current[0] / (BIPOLAR_STEPPER_CURRENT_SENSE_GAIN * 0.15)), 3);
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_STEPPER:
		default:
			return EPHIDGET_UNEXPECTED;
	}
	
	//Make sure values are within defined range, and store to structure
	for (i = 0; i < phid->phid.attr.stepper.numMotors; i++)
	{
		lastPosition[i] = phid->motorPositionEcho[i];
		lastSpeed[i] = phid->motorSpeedEcho[i];
		lastCurrent[i] = phid->motorSensedCurrent[i];

		if(position[i] > phid->motorPositionMax || position[i] < phid->motorPositionMin)
			phid->motorPositionEcho[i] = PUNK_INT64;
		else
			phid->motorPositionEcho[i] = position[i];

		if(speed[i] > phid->motorSpeedMax || speed[i] < -phid->motorSpeedMax)
			LOG(PHIDGET_LOG_WARNING, "Phidget stepper recieved out of range speed data: %lf", speed[i]);
		else
			phid->motorSpeedEcho[i] = speed[i];

		phid->motorSensedCurrent[i] = current[i];

		phid->motorEngagedStateEcho[i] = motorEngaged[i];
	}

	for (i = 0; i < phid->phid.attr.stepper.numInputs; i++)
	{
		lastInput[i] = phid->inputState[i];
		phid->inputState[i] = input[i];
	}

	//make sure phid->motorStoppedState isn't updated until the other data is filled in
	CThread_mutex_lock(&phid->phid.writelock);
	for (i = 0; i < phid->phid.attr.stepper.numMotors; i++)
	{
		//if we are up to date, and the motor is done, set stopped to true - this is the only place that this gets set true;
		justStopped[i] = PFALSE;
		if(phid->packetCounter[i] == phid->packetCounterEcho[i] && motorDone[i] == PTRUE
			&& ((phid->motorPositionEcho[i] == phid->motorPosition[i] && phid->motorSpeedEcho[i] == 0) || phid->motorEngagedStateEcho[i] == PFALSE))
		{
			if(phid->motorStoppedState[i] == PFALSE)
				justStopped[i] = PTRUE;
			phid->motorStoppedState[i] = PTRUE;
		}
		else if(motorDone[i] == PFALSE)
			phid->motorStoppedState[i] = PFALSE;
	}
	//LOG(PHIDGET_LOG_VERBOSE, " ECHO: %d %d %d %d",phid->packetCounterEcho[0],phid->packetCounterEcho[1],phid->packetCounterEcho[2],phid->packetCounterEcho[3]);
	CThread_mutex_unlock(&phid->phid.writelock);

	//send out any events for changed data
	for (i = 0; i < phid->phid.attr.stepper.numInputs; i++)
	{
		if(phid->inputState[i] != PUNK_BOOL && phid->inputState[i] != lastInput[i])
			FIRE(InputChange, i, phid->inputState[i]);
	}

	for (i = 0; i < phid->phid.attr.stepper.numMotors; i++)
	{
		if(phid->motorPositionEcho[i] != PUNK_DBL && phid->motorEngagedStateEcho[i] == PTRUE
			&& (phid->motorPositionEcho[i] != lastPosition[i] || justStopped[i] == PTRUE) )
			FIRE(PositionChange, i, phid->motorPositionEcho[i]);

		if(phid->motorSpeedEcho[i] != PUNK_DBL && phid->motorSpeedEcho[i] != lastSpeed[i])
			FIRE(VelocityChange, i, phid->motorSpeedEcho[i]);

		if(phid->motorSensedCurrent[i] != PUNK_DBL && phid->motorSensedCurrent[i] != lastCurrent[i])
			FIRE(CurrentChange, i, phid->motorSensedCurrent[i]);
	}

	return EPHIDGET_OK;
}

//eventsAfterOpen - sends out an event for all valid data, used during attach initialization
CPHIDGETINITEVENTS(Stepper)
	int i = 0;

	for (i = 0; i<phid->phid.attr.stepper.numInputs; i++)
	{
		if(phid->inputState[i] != PUNK_BOOL)
			FIRE(InputChange, i, phid->inputState[i]);
	}
	for (i = 0; i<phid->phid.attr.stepper.numMotors; i++)
	{
		if(phid->motorSensedCurrent[i] != PUNK_DBL)
			FIRE(CurrentChange, i, phid->motorSensedCurrent[i]);
		if(phid->motorSpeedEcho[i] != PUNK_DBL)
			FIRE(VelocityChange, i, phid->motorSpeedEcho[i]);
		if(phid->motorPositionEcho[i] != PUNK_INT64 && phid->motorEngagedStateEcho[i] == PTRUE)
			FIRE(PositionChange, i, phid->motorPositionEcho[i]);
	}

	return EPHIDGET_OK;
}

//getPacket - used by write thread to get the next packet to send to device
CGETPACKET_BUF(Stepper)

//sendpacket - sends a packet to the device asynchronously, blocking if the 1-packet queue is full
CSENDPACKET_BUF(Stepper)

//makePacket - constructs a packet using current device state
CMAKEPACKETINDEXED(Stepper)
	unsigned char accel = 0, flags = 0;
	unsigned short speed = 0;
	__int64 position = 0;

	int packet_type = Index & 0x30;
	Index = Index & 0x0F;

	TESTPTRS(phid, buffer);

	if(phid->packetCounter[Index] == PUNK_INT)
		phid->packetCounter[Index] = 0;

	phid->packetCounter[Index]++;
	phid->packetCounter[Index] &= 0x0F;
	phid->motorStoppedState[Index] = PFALSE;

	//LOG(PHIDGET_LOG_VERBOSE, " OUT %d: %d",Index, phid->packetCounter[Index]);

	//have to make sure that everything to be sent has some sort of default value if the user hasn't set a value
	if(phid->motorEngagedState[Index] == PUNK_BOOL)
		phid->motorEngagedState[Index] = PFALSE; //note motors are not engaged by default

	if(phid->motorEngagedState[Index] == PFALSE)
		flags |= MOTOR_DISABLED_STEPPER;

	switch(packet_type)
	{
		case STEPPER_POSITION_PACKET:
			break;
		case STEPPER_VEL_ACCEL_PACKET:
			if(phid->motorSpeed[Index] == PUNK_DBL)
				phid->motorSpeed[Index] = phid->motorSpeedMax / 2;
			if(phid->motorAcceleration[Index] == PUNK_DBL)
				phid->motorAcceleration[Index] = phid->accelerationMax / 2; //mid-level acceleration
			break;
		case STEPPER_RESET_PACKET:
			phid->motorPosition[Index] = phid->motorPositionReset[Index];
			break;
		default:
			return EPHIDGET_UNEXPECTED;
	}

	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_UNIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				//2-bit index, 2-bit packet type, 4-bit counter 
				buffer[0] = (Index << 6) | packet_type | phid->packetCounter[Index]; 
				//flags
				buffer[1] = flags; 

				switch(packet_type)
				{
					case STEPPER_POSITION_PACKET:
						position = phid->motorPosition[Index] << 6;
						position += 0x20;

						//48-bit position
						buffer[2] = (unsigned char)(position >> 40);
						buffer[3] = (unsigned char)(position >> 32);
						buffer[4] = (unsigned char)(position >> 24);
						buffer[5] = (unsigned char)(position >> 16);
						buffer[6] = (unsigned char)(position >> 8);
						buffer[7] = (unsigned char)(position);
						break;
					case STEPPER_VEL_ACCEL_PACKET:
						accel = (unsigned char)round((phid->motorAcceleration[Index]/phid->accelerationMax) * 63.0);
						speed = (unsigned short)round((phid->motorSpeed[Index]/phid->motorSpeedMax) * 511.0);

						//6-bit acceleration
						buffer[2] = accel;
						//9-bit speed
						buffer[3] = speed >> 8;
						buffer[4] = speed & 0xFF;
						//not used
						buffer[5] = 0;
						buffer[6] = 0;
						buffer[7] = 0;
						break;
					case STEPPER_RESET_PACKET:
						position = phid->motorPositionReset[Index] << 6;
						position += 0x20;

						//48-bit position
						buffer[2] = (unsigned char)(position >> 40);
						buffer[3] = (unsigned char)(position >> 32);
						buffer[4] = (unsigned char)(position >> 24);
						buffer[5] = (unsigned char)(position >> 16);
						buffer[6] = (unsigned char)(position >> 8);
						buffer[7] = (unsigned char)(position);
						break;
					default:
						return EPHIDGET_UNEXPECTED;
				}
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_BIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				unsigned char currentLimit = 0;
				double Vref;

				//2-bit index, 2-bit packet type, 4-bit counter 
				buffer[0] = (Index << 6) | packet_type | phid->packetCounter[Index]; 
				//flags
				buffer[1] = flags; 

				switch(packet_type)
				{
					case STEPPER_POSITION_PACKET:
						position = phid->motorPosition[Index] << 3;
						position += 0x04;

						//48-bit position
						buffer[2] = (unsigned char)(position >> 40);
						buffer[3] = (unsigned char)(position >> 32);
						buffer[4] = (unsigned char)(position >> 24);
						buffer[5] = (unsigned char)(position >> 16);
						buffer[6] = (unsigned char)(position >> 8);
						buffer[7] = (unsigned char)(position);
						break;
					case STEPPER_VEL_ACCEL_PACKET:
						if(phid->motorCurrentLimit[Index] == PUNK_DBL)
							phid->motorCurrentLimit[Index] = 50;

						accel = (unsigned char)round((phid->motorAcceleration[Index]/phid->accelerationMax) * 255.0);
						speed = (unsigned short)round((phid->motorSpeed[Index]/phid->motorSpeedMax) * 2048.0);

						Vref = phid->motorCurrentLimit[Index] * 8 * BIPOLAR_STEPPER_CURRENT_LIMIT_Rs;
						currentLimit = (unsigned char)round( (((Vref - 2.08)/2.08) * 32) + 31 );

						//8-bit acceleration
						buffer[2] = accel;
						//12-bit speed
						buffer[3] = speed >> 8;
						buffer[4] = speed & 0xFF;
						//6-bit torque
						buffer[5] = currentLimit;
						//not used
						buffer[6] = 0;
						buffer[7] = 0;
						break;
					case STEPPER_RESET_PACKET:
						position = phid->motorPositionReset[Index] << 3;
						position += 0x04;

						//48-bit position
						buffer[2] = (unsigned char)(position >> 40);
						buffer[3] = (unsigned char)(position >> 32);
						buffer[4] = (unsigned char)(position >> 24);
						buffer[5] = (unsigned char)(position >> 16);
						buffer[6] = (unsigned char)(position >> 8);
						buffer[7] = (unsigned char)(position);
						break;
					default:
						return EPHIDGET_UNEXPECTED;
				}
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_STEPPER:
		default:
			return EPHIDGET_UNEXPECTED;
	}

	return EPHIDGET_OK;
}

// === Exported Functions === //

//create and initialize a device structure
CCREATE(Stepper, PHIDDEF_STEPPER)

//event setup functions
CFHANDLE(Stepper, InputChange, int, int)
CFHANDLE(Stepper, PositionChange, int, __int64)
CFHANDLE(Stepper, VelocityChange, int, double)
CFHANDLE(Stepper, CurrentChange, int, double)

CGET(Stepper,InputCount,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	MASGN(phid.attr.stepper.numInputs)
}

CGETINDEX(Stepper,InputState,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numInputs)
	TESTMASGN(inputState[Index], PUNK_BOOL)
	MASGN(inputState[Index])
}

CGET(Stepper,MotorCount,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	MASGN(phid.attr.stepper.numMotors)
}
CGETINDEX(Stepper,Acceleration,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTMASGN(motorAcceleration[Index], PUNK_DBL)
	MASGN(motorAcceleration[Index])
}
CSETINDEX(Stepper,Acceleration,double)
	TESTPTR(phid) 
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTRANGE(phid->accelerationMin, phid->accelerationMax)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(Acceleration, "%f");
	else
		SENDPACKETINDEXED(Stepper, motorAcceleration[Index], Index + STEPPER_VEL_ACCEL_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(Stepper,AccelerationMax,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	MASGN(accelerationMax)
}

CGETINDEX(Stepper,AccelerationMin,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	MASGN(accelerationMin)
}

CGETINDEX(Stepper,Velocity,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTMASGN(motorSpeedEcho[Index], PUNK_DBL)
	MASGN(motorSpeedEcho[Index])
}

CGETINDEX(Stepper,VelocityLimit,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTMASGN(motorSpeed[Index], PUNK_DBL)
	MASGN(motorSpeed[Index])
}
CSETINDEX(Stepper,VelocityLimit,double)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTRANGE(phid->motorSpeedMin, phid->motorSpeedMax)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(VelocityLimit, "%lf");
	else
		SENDPACKETINDEXED(Stepper, motorSpeed[Index], Index + STEPPER_VEL_ACCEL_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(Stepper,VelocityMax,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	MASGN(motorSpeedMax)
}

CGETINDEX(Stepper,VelocityMin,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	MASGN(motorSpeedMin)
}

CGETINDEX(Stepper,TargetPosition,__int64)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTMASGN(motorPosition[Index], PUNK_INT64)
	MASGN(motorPosition[Index])
}
CSETINDEX(Stepper,TargetPosition,__int64)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTRANGE(phid->motorPositionMin, phid->motorPositionMax)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(TargetPosition, "%lld");
	else
		SENDPACKETINDEXED(Stepper, motorPosition[Index], Index + STEPPER_POSITION_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(Stepper,CurrentPosition,__int64)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTMASGN(motorPositionEcho[Index], PUNK_INT64)
	MASGN(motorPositionEcho[Index])
}
CSETINDEX(Stepper,CurrentPosition,__int64)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTRANGE(phid->motorPositionMin, phid->motorPositionMax)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(CurrentPosition, "%lld");
	else
		SENDPACKETINDEXED(Stepper, motorPositionReset[Index], Index + STEPPER_RESET_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(Stepper,PositionMax,__int64)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	MASGN(motorPositionMax)
}

CGETINDEX(Stepper,PositionMin,__int64)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	MASGN(motorPositionMin)
}

CGETINDEX(Stepper,Current,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)

	//only the bipolar has current sense
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_BIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				TESTMASGN(motorSensedCurrent[Index], PUNK_DBL)
				MASGN(motorSensedCurrent[Index])
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_UNIPOLAR_STEPPER:
		case PHIDSPEC_STEPPER:
		default:
			return EPHIDGET_UNSUPPORTED;
	}
}

CGETINDEX(Stepper,CurrentLimit,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)

	//only the bipolar has currentLimit
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_BIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				TESTMASGN(motorCurrentLimit[Index], PUNK_DBL)
				MASGN(motorCurrentLimit[Index])
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_UNIPOLAR_STEPPER:
		case PHIDSPEC_STEPPER:
		default:
			return EPHIDGET_UNSUPPORTED;
	}
}
CSETINDEX(Stepper,CurrentLimit,double)
	TESTPTR(phid)
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTRANGE(phid->currentMin, phid->currentMax)

	//only the bipolar has currentLimit
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_BIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
			{
				if(phid->phid.networkInfo)
					ADDNETWORKKEYINDEXED(CurrentLimit, "%lf");
				else
					SENDPACKETINDEXED(Stepper, motorCurrentLimit[Index], Index + STEPPER_VEL_ACCEL_PACKET);
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_UNIPOLAR_STEPPER:
		case PHIDSPEC_STEPPER:
		default:
			return EPHIDGET_UNSUPPORTED;
	}

	return EPHIDGET_OK;
}

CGETINDEX(Stepper,CurrentMax,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)

	//only the bipolar has current
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_BIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
				MASGN(currentMax)
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_UNIPOLAR_STEPPER:
		case PHIDSPEC_STEPPER:
		default:
			return EPHIDGET_UNSUPPORTED;
	}
}

CGETINDEX(Stepper,CurrentMin,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	//only the bipolar has current
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_BIPOLAR_STEPPER:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 200))
				MASGN(currentMin)
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_UNIPOLAR_STEPPER:
		case PHIDSPEC_STEPPER:
		default:
			return EPHIDGET_UNSUPPORTED;
	}
}

CGETINDEX(Stepper,Engaged,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTMASGN(motorEngagedStateEcho[Index], PUNK_BOOL)
	MASGN(motorEngagedStateEcho[Index])
}
CSETINDEX(Stepper,Engaged,int)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTRANGE(PFALSE, PTRUE)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(Engaged, "%d");
	else
		//hang off the STEPPER_VEL_ACCEL_PACKET because this will have the side effect of also setting these two states
		SENDPACKETINDEXED(Stepper, motorEngagedState[Index], Index + STEPPER_VEL_ACCEL_PACKET);

	return EPHIDGET_OK;
}

CGETINDEX(Stepper,Stopped,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_STEPPER)
	TESTATTACHED
	TESTINDEX(phid.attr.stepper.numMotors)
	TESTMASGN(motorStoppedState[Index], PUNK_BOOL)
	MASGN(motorStoppedState[Index])
}
