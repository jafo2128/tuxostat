#include "stdafx.h"
#include "cphidgetencoder.h"
#include <stdio.h>
#include "cusb.h"
#include "csocket.h"
#include "cthread.h"

// === Internal Functions === //

//clearVars - sets all device variables to unknown state
CPHIDGETCLEARVARS(Encoder)
	int i = 0;
	for (i = 0; i<ENCODER_MAXINPUTS; i++)
	{
		phid->inputState[i] = PUNK_BOOL;
	}
	for (i = 0; i<ENCODER_MAXENCODERS; i++)
	{
		phid->encoderPosition[i] = PUNK_INT; //position set to zero on each attach
		phid->encoderTimeStamp[i] = PUNK_INT;
	}
	return EPHIDGET_OK;
}

//initAfterOpen - sets up the initial state of an object, reading in packets from the device if needed
//				  used during attach initialization - on every attach
CPHIDGETINIT(Encoder)
	int i = 0;

	TESTPTR(phid);

	//set data arrays to unknown, initial states
	for (i = 0; i<phid->phid.attr.encoder.numInputs; i++)
	{
		phid->inputState[i] = PUNK_BOOL;
	}
	for (i = 0; i<phid->phid.attr.encoder.numEncoders; i++)
	{
		phid->encoderPosition[i] = 0; //position set to zero on each attach
		phid->encoderTimeStamp[i] = 0;
	}

	//no sense in issuing a read, it will just block anyways

	return EPHIDGET_OK;
}

//dataInput - parses device packets
CPHIDGETDATA(Encoder)
	int i=0,j=0;

	unsigned char input[ENCODER_MAXINPUTS];
	short positionChange[ENCODER_MAXENCODERS];
	unsigned short timeChange = 0, timeStamp = 0;

	unsigned char lastInputState[ENCODER_MAXINPUTS];

	if (length<0) return EPHIDGET_INVALIDARG;
	TESTPTR(phid);
	TESTPTR(buffer);

	ZEROMEM(input, sizeof(input));
	ZEROMEM(positionChange, sizeof(positionChange));
	ZEROMEM(lastInputState, sizeof(lastInputState));

	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_ENCODER:
			/* OLD version here... */
			if (phid->phid.deviceVersion < 110)
			{
				if (buffer[1] & 0x04)
					input[0] = PFALSE;
				else
					input[0] = PTRUE;

				positionChange[0] = (char)buffer[2];

				timeStamp = (((unsigned short)buffer[4]) << 8) + buffer[3];
			}
			/* NEW version 1-encoder = 1.10+*/
			else if (phid->phid.deviceVersion >= 110 && phid->phid.deviceVersion < 300)
			{
				if (buffer[1] & 0x01)
					input[0] = PFALSE;
				else
					input[0] = PTRUE;
				
				positionChange[0] = (char)buffer[4];

				timeStamp = (((unsigned short)buffer[3]) << 8) + buffer[2];
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_ENCODER_4:
			/* 4-encoder = 2.00+ */
			if (phid->phid.deviceVersion >= 200 && phid->phid.deviceVersion < 300)
			{
				for (i = 0, j = 1; i<(phid->phid.attr.encoder.numInputs); i++, j<<=1)
				{
					if (buffer[1] & j)
						input[i] = PFALSE;
					else
						input[i] = PTRUE;
				}
				
				for(i=0;i<phid->phid.attr.encoder.numEncoders;i++)
					positionChange[i] = (char)buffer[i+4];

				timeStamp = (((unsigned short)buffer[3]) << 8) + buffer[2];
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		case PHIDSPEC_ENCODER_HS:
			/* high speed encoder */
			if (phid->phid.deviceVersion >= 300 && phid->phid.deviceVersion < 400) {

				//this will work for 8 inputs before we need to change the protocol
				//currently no high-speed encoder has any inputs
				for (i = 0, j = 1; i < (phid->phid.attr.encoder.numInputs); i++, j<<=1)
				{
					if (buffer[1] & j)
						input[i] = PFALSE;
					else
						input[i] = PTRUE;
				}
				
				//this will work for two encoders before we need to change the protocol
				for(i=0;i<phid->phid.attr.encoder.numEncoders;i++) {
					positionChange[i] = (((unsigned short)buffer[2*i+5]) << 8) + buffer[2*i+4];
				}

				timeStamp = (((unsigned short)buffer[3]) << 8) + buffer[2];
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		default:
			return EPHIDGET_UNEXPECTED;
	}

	//Make sure values are within defined range, and store to structure
	for (i = 0; i < phid->phid.attr.encoder.numInputs; i++)
	{
		lastInputState[i] = phid->inputState[i];
		phid->inputState[i] = input[i];
	}
	for(i=0; i < phid->phid.attr.encoder.numEncoders; i++) 
	{
		if (positionChange[i])
		{
			phid->encoderPosition[i] += positionChange[i];

			timeChange = (timeStamp - phid->encoderTimeStamp[i]);

			if (timeChange > 30000) timeChange = 30000;

			phid->encoderTimeStamp[i] = timeStamp;
		}
	}

	//send out any events for changed data
	for (i = 0; i < phid->phid.attr.encoder.numInputs; i++)
	{
		if(phid->inputState[i] != lastInputState[i])
			FIRE(InputChange, i, phid->inputState[i]);
	}
	for(i=0; i < phid->phid.attr.encoder.numEncoders; i++) 
	{
		if(positionChange[i] != 0)
			FIRE(PositionChange, i, (unsigned short)timeChange, positionChange[i]);
	}

	return EPHIDGET_OK;
}

//eventsAfterOpen - sends out an event for all valid data, used during attach initialization
CPHIDGETINITEVENTS(Encoder)
	//no initial events for encoder - position always starts at 0
	phid = 0;
	return EPHIDGET_OK;
}

//getPacket - not used for encoder
CGETPACKET(Encoder)
	return EPHIDGET_UNEXPECTED;
}

// === Exported Functions === //

//create and initialize a device structure
CCREATE(Encoder, PHIDDEF_ENCODER)

//event setup functions
CFHANDLE(Encoder, InputChange, int, int)
CFHANDLE(Encoder, PositionChange, int, int, int)

CGET(Encoder,InputCount,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ENCODER)
	TESTATTACHED
	MASGN(phid.attr.encoder.numInputs)
}

CGETINDEX(Encoder,InputState,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ENCODER)
	TESTATTACHED
	TESTINDEX(phid.attr.encoder.numInputs)
	TESTMASGN(inputState[Index], PUNK_BOOL)
	MASGN(inputState[Index])
}

CGET(Encoder,EncoderCount,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ENCODER)
	TESTATTACHED
	MASGN(phid.attr.encoder.numEncoders)
}

CGETINDEX(Encoder,Position,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_ENCODER)
	TESTATTACHED
	TESTINDEX(phid.attr.encoder.numEncoders)
	TESTMASGN(encoderPosition[Index], PUNK_INT)
	MASGN(encoderPosition[Index])
}
CSETINDEX(Encoder,Position,int)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_ENCODER)
	TESTATTACHED
	TESTINDEX(phid.attr.encoder.numEncoders)
	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(ResetPosition, "%d");
	else
		phid->encoderPosition[Index] = newVal;
	return EPHIDGET_OK;
}

// === Deprecated Functions === //

CGETINDEX(Encoder,EncoderPosition,int)
	return CPhidgetEncoder_getPosition(phid, Index, pVal);
}
CSETINDEX(Encoder,EncoderPosition,int)
	return CPhidgetEncoder_setPosition(phid, Index, newVal);
}
CGET(Encoder,NumInputs,int)
	return CPhidgetEncoder_getInputCount(phid, pVal);
}
CGET(Encoder,NumEncoders,int)
	return CPhidgetEncoder_getEncoderCount(phid, pVal);
}
