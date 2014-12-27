#include "stdafx.h"
#include "cphidgetservo.h"
#include "cusb.h"
#include "csocket.h"
#include "cthread.h"

// === Internal Functions === //

//clearVars - sets all device variables to unknown state
CPHIDGETCLEARVARS(Servo)
	int i = 0;

	phid->motorPositionMax = PUNK_DBL;
	phid->motorPositionMin = PUNK_DBL;
	phid->fullStateEcho = PUNK_BOOL;

	for (i = 0; i<SERVO_MAXSERVOS; i++)
	{
		phid->motorPosition[i] = PUNK_DBL;
		phid->motorEngagedState[i] = PUNK_BOOL;
		phid->motorPositionEcho[i] = PUNK_DBL;
		phid->motorEngagedStateEcho[i] = PUNK_BOOL;
	}

	return EPHIDGET_OK;
}

//initAfterOpen - sets up the initial state of an object, reading in packets from the device if needed
//				  used during attach initialization - on every attach
CPHIDGETINIT(Servo)
	int i = 0;

	TESTPTR(phid);

	//Setup max/min values
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_SERVO_1MOTOR:
		case PHIDSPEC_SERVO_4MOTOR:
			if (phid->phid.deviceVersion == 200)
			{
				phid->motorPositionMax = 232;
				phid->motorPositionMin = 0;
				phid->fullStateEcho = PFALSE;
			}
			else if ((phid->phid.deviceVersion >= 300) && (phid->phid.deviceVersion < 313))
			{
				phid->motorPositionMax = 232;
				phid->motorPositionMin = -22.9;
				phid->fullStateEcho = PFALSE;
			}
			else if ((phid->phid.deviceVersion >= 313) && (phid->phid.deviceVersion < 400))
			{
				phid->motorPositionMax = 232;
				phid->motorPositionMin = -22.9;
				phid->fullStateEcho = PTRUE;
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_SERVO_1MOTOR_OLD:
		case PHIDSPEC_SERVO_4MOTOR_OLD:
			if (phid->phid.deviceVersion == 200)
			{
				phid->motorPositionMax = 232;
				phid->motorPositionMin = 0;
				phid->fullStateEcho = PFALSE;
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		default:
			return EPHIDGET_UNEXPECTED;
	}
	
	//set data arrays to unknown
	for (i = 0; i<phid->phid.attr.servo.numMotors; i++)
	{
		phid->motorPositionEcho[i] = PUNK_DBL;
		phid->motorEngagedStateEcho[i] = PUNK_BOOL;
	}

	//issue a read for devices that return output data
	if(phid->fullStateEcho)
	{
		CPhidget_read((CPhidgetHandle)phid);
	}

	//recover what we can - if anything isn't filled out, it's PUNK anyways
	for (i = 0; i<phid->phid.attr.servo.numMotors; i++)
	{
		phid->motorPosition[i] = phid->motorPositionEcho[i];
		phid->motorEngagedState[i] = phid->motorEngagedStateEcho[i];
	}

	return EPHIDGET_OK;
}

//dataInput - parses device packets
CPHIDGETDATA(Servo)
	int i = 0;
	double position[SERVO_MAXSERVOS];
	double lastPosition[SERVO_MAXSERVOS];

	if (length<0) return EPHIDGET_INVALIDARG;
	TESTPTR(phid);
	TESTPTR(buffer);

	ZEROMEM(position, sizeof(position));
	ZEROMEM(lastPosition, sizeof(lastPosition));

	//Parse device packets - store data locally
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_SERVO_1MOTOR:
			if ((phid->phid.deviceVersion >= 313) && (phid->phid.deviceVersion < 400))
			{
					position[0] = (((unsigned short)buffer[0]) << 5) + buffer[1];
					position[0] = round_double(((position[0] / 10.6) - 23.0), 2);
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_SERVO_4MOTOR:
			if ((phid->phid.deviceVersion >= 313) && (phid->phid.deviceVersion < 400))
			{
				for (i = 0; i < phid->phid.attr.servo.numMotors; i++)
				{
					position[i] = (((unsigned short)buffer[i*2]) << 5) + buffer[(i*2) + 1];
					position[i] = round_double(((position[i] / 10.6) - 23.0), 2);
				}
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_SERVO_1MOTOR_OLD:
		case PHIDSPEC_SERVO_4MOTOR_OLD:
			return EPHIDGET_UNSUPPORTED;
		default:
			return EPHIDGET_UNEXPECTED;
	}

	//Make sure values are within defined range, and store to structure
	for (i = 0; i < phid->phid.attr.servo.numMotors; i++)
	{
		lastPosition[i] = phid->motorPositionEcho[i];
		if(position[i] > phid->motorPositionMax || position[i] < phid->motorPositionMin)
		{
			if(position[i] == -23.0)
				phid->motorEngagedStateEcho[i] = PFALSE;
			else
				phid->motorEngagedStateEcho[i] = PUNK_BOOL;
			phid->motorPositionEcho[i] = PUNK_DBL;
		}
		else
		{
			phid->motorPositionEcho[i] = position[i];
			phid->motorEngagedStateEcho[i] = PTRUE;
		}
	}
	
	//send out any events for changed data
	for (i = 0; i < phid->phid.attr.servo.numMotors; i++)
	{
		if(phid->motorPositionEcho[i] != PUNK_DBL && phid->motorPositionEcho[i] != lastPosition[i] && phid->motorEngagedStateEcho[i] == PTRUE)
		{
			FIRE(PositionChange, i, phid->motorPositionEcho[i]);
			//Deprecated
			FIRE(MotorPositionChange, i, phid->motorPositionEcho[i]);
		}
	}

	return EPHIDGET_OK;
}

//eventsAfterOpen - sends out an event for all valid data, used during attach initialization
CPHIDGETINITEVENTS(Servo)
	int i = 0;

	for (i = 0; i < phid->phid.attr.servo.numMotors; i++)
	{
		if(phid->motorPositionEcho[i] != PUNK_DBL && phid->motorEngagedStateEcho[i] == PTRUE)
		{
			FIRE(PositionChange, i, phid->motorPositionEcho[i]);
			//Deprecated
			FIRE(MotorPositionChange, i, phid->motorPositionEcho[i]);
		}
	}

	return EPHIDGET_OK;
}

//getPacket - used by write thread to get the next packet to send to device
CGETPACKET_BUF(Servo)

//sendpacket - sends a packet to the device asynchronously, blocking if the 1-packet queue is full
CSENDPACKET_BUF(Servo)

//makePacket - constructs a packet using current device state
CMAKEPACKETINDEXED(Servo)
	int i = 0;

	TESTPTRS(phid, buffer);

	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_SERVO_1MOTOR:
			if (phid->phid.deviceVersion == 200) //this version may not exist in this product id
			{
				buffer[0] = 0;
				if(phid->motorPosition[0] == PUNK_DBL || phid->motorEngagedState[0] == PFALSE)
					buffer[1] = (unsigned char)-23.0;
				else
					buffer[1] = (unsigned char)round(phid->motorPosition[Index]);
			}
			else if ((phid->phid.deviceVersion >= 300) && (phid->phid.deviceVersion < 400))
			{
				int microsecondPulse = 0;

				if(phid->motorPosition[0] == PUNK_DBL || phid->motorEngagedState[0] == PFALSE)
					microsecondPulse = 0;
				else
					microsecondPulse = (int)((phid->motorPosition[0] + 23.0) * 10.6);

				buffer[0] = (unsigned char)(microsecondPulse & 0xFF);
				buffer[1] = (unsigned char)(microsecondPulse >> 8);
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_SERVO_4MOTOR:
			if (phid->phid.deviceVersion == 200) //this version may not exist in this product id
			{
				if (Index == 0) buffer[0] = 2;
				if (Index == 1) buffer[0] = 3;
				if (Index == 2) buffer[0] = 0;
				if (Index == 3) buffer[0] = 1;
				if(phid->motorPosition[Index] == PUNK_DBL || phid->motorEngagedState[Index] == PFALSE)
					buffer[1] = (unsigned char)-23.0;
				else
					buffer[1] = (unsigned char)round(phid->motorPosition[Index]);
			}
			else if ((phid->phid.deviceVersion >= 300) && (phid->phid.deviceVersion < 400))
			{
				int microsecondPulse[4];

				ZEROMEM(microsecondPulse, sizeof(microsecondPulse));

				for (i = 0; i<phid->phid.attr.servo.numMotors; i++)
				{
					if(phid->motorPosition[i] == PUNK_DBL || phid->motorEngagedState[i] == PFALSE)
					{
						microsecondPulse[i] = 0;
						phid->motorEngagedState[i] = PFALSE;
						if (!(phid->fullStateEcho))
							phid->motorEngagedStateEcho[i] = PFALSE;
					}
					else
					{
						microsecondPulse[i] = (int)((phid->motorPosition[i] + 23.0) * 10.6);
						phid->motorEngagedState[i] = PTRUE;
						if (!(phid->fullStateEcho))
							phid->motorEngagedStateEcho[i] = PTRUE;
					}
				}

				buffer[0] = (unsigned char)(microsecondPulse[0] & 0xFF);
				buffer[1] = (unsigned char)((microsecondPulse[0] >> 8) & 0x0F);

				buffer[2] = (unsigned char)(microsecondPulse[1] & 0xFF);
				buffer[1] |= (unsigned char)((microsecondPulse[1] >> 4) & 0xF0);

				buffer[3] = (unsigned char)(microsecondPulse[2] & 0xFF);
				buffer[4] = (unsigned char)((microsecondPulse[2] >> 8) & 0x0F);

				buffer[5] = (unsigned char)(microsecondPulse[3] & 0xFF);
				buffer[4] |= (unsigned char)((microsecondPulse[3] >> 4) & 0xF0);
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_SERVO_1MOTOR_OLD:
			if (phid->phid.deviceVersion == 200)
			{
				buffer[0] = 0;
				if(phid->motorPosition[0] == PUNK_DBL || phid->motorEngagedState[0] == PFALSE)
					buffer[1] = (unsigned char)-23.0;
				else
					buffer[1] = (unsigned char)round(phid->motorPosition[Index]);
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_SERVO_4MOTOR_OLD:
			if (phid->phid.deviceVersion == 200)
			{
				if (Index == 0) buffer[0] = 2;
				if (Index == 1) buffer[0] = 3;
				if (Index == 2) buffer[0] = 0;
				if (Index == 3) buffer[0] = 1;
				if(phid->motorPosition[Index] == PUNK_DBL || phid->motorEngagedState[Index] == PFALSE)
					buffer[1] = (unsigned char)-23.0;
				else
					buffer[1] = (unsigned char)round(phid->motorPosition[Index]);
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
CCREATE(Servo, PHIDDEF_SERVO)

//event setup functions
CFHANDLE(Servo, PositionChange, int, double)

CGET(Servo,MotorCount,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_SERVO)
	TESTATTACHED
	MASGN(phid.attr.servo.numMotors)
}

CGETINDEX(Servo,Position,double)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_SERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.servo.numMotors)
	TESTMASGN(motorPositionEcho[Index], PUNK_DBL)
	if(phid->motorEngagedState[Index] != PTRUE) {*pVal = PUNK_DBL; return EPHIDGET_UNKNOWNVAL;}
	MASGN(motorPositionEcho[Index])
}
CSETINDEX(Servo,Position,double)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_SERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.servo.numMotors)
	TESTRANGE(phid->motorPositionMin, phid->motorPositionMax)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(Position, "%lf");
	else
	{
		//Note: setting a position will always engage a servo
		phid->motorEngagedState[Index] = PTRUE;
		SENDPACKETINDEXED(Servo, motorPosition[Index], Index);
		if (!(phid->fullStateEcho))
		{
			phid->motorEngagedStateEcho[Index] = PTRUE;
			if (phid->motorPositionEcho[Index] == PUNK_BOOL || phid->motorPositionEcho[Index] != newVal)
			{
				phid->motorPositionEcho[Index] = newVal;
				{
					FIRE(PositionChange, Index, newVal);
					//Deprecated
					FIRE(MotorPositionChange, Index, newVal);
				}
			}
		}
	}

	return EPHIDGET_OK;
}

CGETINDEX(Servo,PositionMax,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_SERVO)
	TESTATTACHED
	MASGN(motorPositionMax)
}

CGETINDEX(Servo,PositionMin,double)
	TESTPTRS(phid,pVal) 	
	TESTDEVICETYPE(PHIDDEF_SERVO)
	TESTATTACHED
	MASGN(motorPositionMin)
}

CGETINDEX(Servo,Engaged,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_SERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.servo.numMotors)
	TESTMASGN(motorEngagedState[Index], PUNK_BOOL)
	MASGN(motorEngagedState[Index])
}
CSETINDEX(Servo,Engaged,int)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_SERVO)
	TESTATTACHED
	TESTINDEX(phid.attr.servo.numMotors)
	TESTRANGE(PFALSE, PTRUE)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(Engaged, "%d");
	else
		SENDPACKETINDEXED(Servo, motorEngagedState[Index], Index);

	return EPHIDGET_OK;
}

// === Deprecated Functions === //

CFHANDLE(Servo, MotorPositionChange, int, double)
CGET(Servo,NumMotors,int)
	return CPhidgetServo_getMotorCount(phid, pVal);
}
CGETINDEX(Servo,MotorPosition,double)
	return CPhidgetServo_getPosition(phid, Index, pVal);
}
CSETINDEX(Servo,MotorPosition,double)
	return CPhidgetServo_setPosition(phid, Index, newVal);
}
CGETINDEX(Servo,MotorPositionMax,double)
	return CPhidgetServo_getPositionMax(phid, Index, pVal);
}
CGETINDEX(Servo,MotorPositionMin,double)
	return CPhidgetServo_getPositionMin(phid, Index, pVal);
}
CGETINDEX(Servo,MotorOn,int)
	return CPhidgetServo_getEngaged(phid, Index, pVal);
}
CSETINDEX(Servo,MotorOn,int)
	return CPhidgetServo_setEngaged(phid, Index, newVal);
}
