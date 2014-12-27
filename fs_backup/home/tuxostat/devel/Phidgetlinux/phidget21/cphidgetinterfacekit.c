#include "stdafx.h"
#include "cphidgetinterfacekit.h"
#include "cusb.h"
#include "csocket.h"
#include "cthread.h"

// === Internal Functions === //

static int CPhidgetInterfaceKit_getPacket(CPhidgetHandle, unsigned char *, unsigned int *);

//clearVars - sets all device variables to unknown state
CPHIDGETCLEARVARS(InterfaceKit)
	int i = 0;

	phid->ratiometric = PUNK_BOOL;
	phid->lastChangedOutput = PUNK_INT;
	phid->fullStateEcho = PUNK_BOOL;

	for (i = 0; i<IFKIT_MAXINPUTS; i++)
		phid->physicalState[i] = PUNK_BOOL;
	for (i = 0; i<IFKIT_MAXSENSORS; i++)
	{
		phid->sensorChangeTrigger[i] = PUNK_INT;
		phid->sensorRawValue[i] = PUNK_INT;
		phid->sensorValue[i] = PUNK_INT;
		phid->sensorLastValue[i] = PUNK_INT;
	}
	for (i = 0; i<IFKIT_MAXOUTPUTS; i++)
	{
		phid->nextOutputStates[i] = PUNK_BOOL;
		phid->outputEchoStates[i] = PUNK_BOOL;
		phid->outputStates[i] = PUNK_BOOL;
		phid->changedOutputs[i] = PUNK_BOOL;
	}

	return EPHIDGET_OK;
}

//initAfterOpen - sets up the initial state of an object, reading in packets from the device if needed
//				  used during attach initialization - on every attach
CPHIDGETINIT(InterfaceKit)
	int i = 0, j = 0;
	int readtries = 0;
	unsigned char buffer[8] = { 0 };
	unsigned int len = 8;
	int result;

	TESTPTR(phid);

	//initialize triggers, set data arrays to unknown
	phid->ratiometric = PUNK_BOOL;
	for (j = 0; j<phid->phid.attr.ifkit.numInputs; j++)
		phid->physicalState[j] = PUNK_BOOL;
	for (j = 0; j<phid->phid.attr.ifkit.numSensors; j++)
	{
		phid->sensorChangeTrigger[j] = 10;
		phid->sensorRawValue[j] = PUNK_INT;
		phid->sensorValue[j] = PUNK_INT;
		phid->sensorLastValue[j] = PUNK_INT;
	}
	for (j = 0; j<phid->phid.attr.ifkit.numOutputs; j++)
	{
		phid->nextOutputStates[j] = PUNK_BOOL;
		phid->outputEchoStates[j] = PUNK_BOOL;
		phid->changedOutputs[j] = PFALSE;
	}
	phid->lastChangedOutput = 0;

	//init device specific attrs
	phid->phid.awdc_enabled = FALSE;
	phid->fullStateEcho = FALSE;
	switch(phid->phid.deviceIDSpec) {
		case PHIDSPEC_INTERFACEKIT_8_8_8:
			if (phid->phid.deviceVersion >= 821 && phid->phid.deviceVersion < 900) {
				phid->phid.awdc_enabled = PTRUE;
				phid->fullStateEcho = TRUE;
			}
			break;
		case PHIDSPEC_888_with_lcd:	
			if (phid->phid.deviceVersion >= 120 && phid->phid.deviceVersion < 200) {
				phid->phid.awdc_enabled = PTRUE;
				phid->fullStateEcho = TRUE;
			}
			break;
		case PHIDSPEC_INTERFACEKIT_0_0_4:
			if (phid->phid.deviceVersion >= 704 && phid->phid.deviceVersion < 800)
				phid->fullStateEcho = TRUE;
			break;
		case PHIDSPEC_INTERFACEKIT_0_0_8:
			phid->fullStateEcho = TRUE;
			break;
		case PHIDSPEC_INTERFACEKIT_0_16_16:
			if (phid->phid.deviceVersion >= 601 && phid->phid.deviceVersion < 700)
				phid->fullStateEcho = TRUE;
			break;
		case PHIDSPEC_ROTARY_TOUCH:
		case PHIDSPEC_LINEAR_TOUCH:
		case PHIDSPEC_INTERFACEKIT_0_5_7:
		case PHIDSPEC_INTERFACEKIT_0_8_8:
		case PHIDSPEC_INTERFACEKIT_4_8_8:
			break;
	}

	//send out any initial pre-read packets
	switch(phid->phid.deviceIDSpec) {
		case PHIDSPEC_ROTARY_TOUCH:
			{
				//performs initial calibration
				buffer[0] = 0x01;
				if ((result = CUSBSendPacket((CPhidgetHandle)phid, buffer)) != EPHIDGET_OK)
					return result;
			}
			break;
		case PHIDSPEC_LINEAR_TOUCH:
			{
				//performs initial calibration
				buffer[0] = 0x01;
				if ((result = CUSBSendPacket((CPhidgetHandle)phid, buffer)) != EPHIDGET_OK)
					return result;
				SLEEP(100);
				buffer[0] = 0x02;
				if ((result = CUSBSendPacket((CPhidgetHandle)phid, buffer)) != EPHIDGET_OK)
					return result;
			}
			break;
		case PHIDSPEC_INTERFACEKIT_8_8_8:
		case PHIDSPEC_888_with_lcd:	
		case PHIDSPEC_INTERFACEKIT_0_0_4:
		case PHIDSPEC_INTERFACEKIT_0_0_8:
		case PHIDSPEC_INTERFACEKIT_0_16_16:
		case PHIDSPEC_INTERFACEKIT_0_5_7:
		case PHIDSPEC_INTERFACEKIT_0_8_8:
		case PHIDSPEC_INTERFACEKIT_4_8_8:
		default:
			break;
	}

	//read in device state - don't issue reads on devices that block
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_INTERFACEKIT_0_16_16:
		case PHIDSPEC_INTERFACEKIT_0_0_8:
		case PHIDSPEC_INTERFACEKIT_0_0_4:
			if(phid->fullStateEcho)
				CPhidget_read((CPhidgetHandle)phid);
			break;
		case PHIDSPEC_INTERFACEKIT_8_8_8:
		case PHIDSPEC_888_with_lcd:
			readtries = 4;
			while(readtries-- > 0)
			{
				CPhidget_read((CPhidgetHandle)phid);
				for (i = 0; i<phid->phid.attr.ifkit.numSensors; i++)
					if(phid->sensorValue[i] == PUNK_INT)
						break;
				if(i==phid->phid.attr.ifkit.numSensors) break;
			}
			break;
		case PHIDSPEC_LINEAR_TOUCH:
		case PHIDSPEC_ROTARY_TOUCH:
			CPhidget_read((CPhidgetHandle)phid); //clear any bad data...
			CPhidget_read((CPhidgetHandle)phid);
			break;
		//these are all old, maybe deal with them later
		case PHIDSPEC_INTERFACEKIT_0_8_8:
		case PHIDSPEC_INTERFACEKIT_4_8_8:
		case PHIDSPEC_INTERFACEKIT_0_5_7:
		default:
			break;
	}

	//initialize outputs
	for (j = 0; j<phid->phid.attr.ifkit.numOutputs; j++) {
		phid->outputStates[j] = phid->outputEchoStates[j];
	}

	//send out any initial post-read packets
	switch(phid->phid.deviceIDSpec) {
		case PHIDSPEC_INTERFACEKIT_8_8_8:
		case PHIDSPEC_888_with_lcd:
			//always turn on ratiometric because there is no way to read it's state from the device, and most
			// sensors need ratiometric
			phid->ratiometric = PTRUE;
			if ((result = CPhidgetInterfaceKit_getPacket((CPhidgetHandle)phid, (unsigned char *)&buffer, &len)) != EPHIDGET_OK)
				return result;
			if ((result = CUSBSendPacket((CPhidgetHandle)phid, buffer)) != EPHIDGET_OK)
				return result;
			break;
		case PHIDSPEC_ROTARY_TOUCH:
		case PHIDSPEC_LINEAR_TOUCH:
		case PHIDSPEC_INTERFACEKIT_0_0_4:
		case PHIDSPEC_INTERFACEKIT_0_0_8:
		case PHIDSPEC_INTERFACEKIT_0_16_16:
		case PHIDSPEC_INTERFACEKIT_0_5_7:
		case PHIDSPEC_INTERFACEKIT_0_8_8:
		case PHIDSPEC_INTERFACEKIT_4_8_8:
		default:
			break;
	}

	return EPHIDGET_OK;
}

//dataInput - parses device packets
CPHIDGETDATA(InterfaceKit)
	int j = 0, i = 0, k = 0;

	unsigned char outputState[IFKIT_MAXOUTPUTS], lastOutputState[IFKIT_MAXOUTPUTS];
	unsigned char inputState[IFKIT_MAXINPUTS], lastInputState[IFKIT_MAXINPUTS];
	int sensorValue[IFKIT_MAXSENSORS];
	int sensorRawValue[IFKIT_MAXSENSORS];

	if (length<0) return EPHIDGET_INVALIDARG;
	TESTPTR(phid);
	TESTPTR(buffer);

	for (j = 0; j<phid->phid.attr.ifkit.numInputs; j++)
	{
		inputState[j] = PUNK_BOOL;
		lastInputState[j] = phid->physicalState[j];
	}
	for (j = 0; j<phid->phid.attr.ifkit.numSensors; j++)
	{
		sensorRawValue[j] = PUNK_INT;
		sensorValue[j] = PUNK_INT;
	}
	for (j = 0; j<phid->phid.attr.ifkit.numOutputs; j++)
	{
		outputState[j] = PUNK_BOOL;
		lastOutputState[j] = phid->outputEchoStates[j];
	}
		
	//Parse device packets - store data locally
	switch (phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_INTERFACEKIT_4_8_8:
			//Sensors
			sensorRawValue[0] = ((unsigned char)buffer[3] + ((unsigned char)buffer[2] & 0x0f) * 256);
			sensorRawValue[1] = ((unsigned char)buffer[1] + ((unsigned char)buffer[2] & 0xf0) * 16);
			sensorRawValue[2] = ((unsigned char)buffer[6] + ((unsigned char)buffer[5] & 0x0f) * 256);
			sensorRawValue[3] = ((unsigned char)buffer[4] + ((unsigned char)buffer[5] & 0xf0) * 16);

			for (i = 0; i<phid->phid.attr.ifkit.numSensors; i++)
				sensorValue[i] = round((double)sensorRawValue[i] / 4.095);

			//Inputs
			for (i = 0, j = 0x80; i < 4; i++, j >>= 1)
			{
				if ((buffer[0] & j) != 0)
					inputState[i] = PFALSE;
				else
					inputState[i] = PTRUE;
			}
			for (i = 4, j = 0x01; i < 8; i++, j <<= 1)
			{
				if ((buffer[0] & j) != 0)
					inputState[i] = PFALSE;
				else
					inputState[i] = PTRUE;
			}

			break;
		case PHIDSPEC_INTERFACEKIT_0_16_16:
			//Inputs
			for (k = 0; k<2; k++)
			{
				for (i = 0, j = 0x01; i<8; i++, j <<= 1)
				{
					if ((buffer[k] & j) == 0)
						inputState[i+k*8] = PFALSE;
					else
						inputState[i+k*8] = PTRUE;
				}
			}

			//Outputs
			if (phid->fullStateEcho)
			{
				if(phid->phid.deviceVersion == 601) //top 8 bits are flipped
				{
					for (i = 0, j = 0x01; i<8; i++, j <<= 1)
					{
						if ((buffer[2] & j) == 0)
							outputState[i] = PFALSE;
						else
							outputState[i] = PTRUE;
					}
					for (i = 7, j = 0x01; i>=0; i--, j <<= 1)
					{
						if ((buffer[3] & j) == 0)
							outputState[i+8] = PFALSE;
						else
							outputState[i+8] = PTRUE;
					}
				}
				else
				{
					for (k = 0; k<2; k++)
					{
						for (i = 0, j = 0x01; i<8; i++, j <<= 1)
						{
							if ((buffer[k+2] & j) == 0)
								outputState[i+k*8] = PFALSE;
							else
								outputState[i+k*8] = PTRUE;
						}
					}
				}
			}

			break;
		case PHIDSPEC_INTERFACEKIT_8_8_8:
		case PHIDSPEC_888_with_lcd:
			//Inputs
			for (i = 0, j = 0x01; i < phid->phid.attr.ifkit.numInputs; i++, j <<= 1)
			{
				if (buffer[1] & j)
					inputState[i] = PFALSE;
				else
					inputState[i] = PTRUE;
			}

			//there are two types of packets
			if (!(buffer[0] & 0x01))
			{
				//Sensors
				sensorRawValue[0] = ((unsigned char)buffer[2] + ((unsigned char)buffer[3] & 0x0f) * 256);
				sensorRawValue[1] = ((unsigned char)buffer[4] + ((unsigned char)buffer[3] & 0xf0) * 16);
				sensorRawValue[2] = ((unsigned char)buffer[5] + ((unsigned char)buffer[6] & 0x0f) * 256);
				sensorRawValue[3] = ((unsigned char)buffer[7] + ((unsigned char)buffer[6] & 0xf0) * 16);
				for (i = 0; i<4; i++)
					sensorValue[i] = round((double)sensorRawValue[i] / 4.095);

				//Outputs
				if (phid->fullStateEcho)
				{
					for (i = 0, j = 0x10; i<4; i++, j <<= 1)
					{
						if ((buffer[0] & j) == 0)
							outputState[i] = PFALSE;
						else
							outputState[i] = PTRUE;
					}
				}
			}
			else
			{
				//Sensors
				sensorRawValue[4] = ((unsigned char)buffer[2] + ((unsigned char)buffer[3] & 0x0f) * 256);
				sensorRawValue[5] = ((unsigned char)buffer[4] + ((unsigned char)buffer[3] & 0xf0) * 16);
				sensorRawValue[6] = ((unsigned char)buffer[5] + ((unsigned char)buffer[6] & 0x0f) * 256);
				sensorRawValue[7] = ((unsigned char)buffer[7] + ((unsigned char)buffer[6] & 0xf0) * 16);
				for (i = 4; i<8; i++)
					sensorValue[i] = round((double)sensorRawValue[i] / 4.095);

				//Outputs
				if (phid->fullStateEcho)
				{
					for (i = 4, j = 0x10; i<8; i++, j <<= 1)
					{
						if ((buffer[0] & j) == 0)
							outputState[i] = PFALSE;
						else
							outputState[i] = PTRUE;
					}
				}
			}

			break;
		case PHIDSPEC_INTERFACEKIT_0_5_7:
			//Inputs
			for (i = 0, j = 0x01; i<5; i++, j <<= 1)
			{
				if ((buffer[0] & j) == 0)
					inputState[i] = PFALSE;
				else
					inputState[i] = PTRUE;
			}
			break;
		case PHIDSPEC_INTERFACEKIT_0_8_8:
			//Inputs
			for (i = 0, j = 0x01; i<8; i++, j <<= 1)
			{
				if ((buffer[0] & j) == 0)
					inputState[i] = PFALSE;
				else
					inputState[i] = PTRUE;
			}
			break;
		case PHIDSPEC_INTERFACEKIT_0_0_4:
		case PHIDSPEC_INTERFACEKIT_0_0_8:
			//Outputs
			if (phid->fullStateEcho)
			{
				for (i = 0, j = 0x01; i < phid->phid.attr.ifkit.numOutputs; i++, j <<= 1)
				{
					if ((buffer[0] & j) == 0)
						outputState[i] = PFALSE;
					else
						outputState[i] = PTRUE;
				}
			}
			break;
		case PHIDSPEC_ROTARY_TOUCH:
		case PHIDSPEC_LINEAR_TOUCH:
			//Top bit of buffer[0] indicates a touch
			if (buffer[0] & 0x80) //touch detected
			{
				inputState[0] = PTRUE;
				inputState[1] = PTRUE;

				sensorRawValue[0] = buffer[0] & 0x7f;
				sensorValue[0] = (int)((double)sensorRawValue[0] * 7.875);
			}
			else //no touch detected
			{
				//set sensorValue back to unknown
				phid->sensorRawValue[0] = PUNK_INT;
				phid->sensorValue[0] = PUNK_INT;
			
				//Touch
				inputState[0] = PFALSE;

				//Proximity
				if (buffer[0] & 0x01)
					inputState[1] = PTRUE;
				else
					inputState[1] = PFALSE;
			}
			break;
		default:
			return EPHIDGET_UNEXPECTED;
	}

	//Make sure values are within defined range, and store to structure
	for (i = 0; i<phid->phid.attr.ifkit.numSensors; i++)
	{
		if(sensorValue[i] != PUNK_INT && sensorRawValue[i] != PUNK_INT)
		{
			phid->sensorRawValue[i] = sensorRawValue[i];
			if(sensorValue[i] < 0) sensorValue[i] = 0;
			if(sensorValue[i] > 1000) sensorValue[i] = 1000;
			phid->sensorValue[i] = sensorValue[i];
		}
	}
	for (i = 0; i<phid->phid.attr.ifkit.numInputs; i++)
	{
		if(inputState[i] != PUNK_BOOL)
			phid->physicalState[i] = inputState[i];
	}
	for (i = 0; i<phid->phid.attr.ifkit.numOutputs; i++)
	{
		if(outputState[i] != PUNK_BOOL)
		{
			phid->outputEchoStates[i] = outputState[i];
			CThread_mutex_lock(&phid->phid.outputLock);
			if (phid->outputStates[i] == PUNK_BOOL)
				phid->outputStates[i] = outputState[i];
			CThread_mutex_unlock(&phid->phid.outputLock);
		}
	}

	//send out any events for changed data
	for (i = 0; i < phid->phid.attr.ifkit.numSensors; i++)
	{	
		if(sensorValue[i] != PUNK_INT)
		{
			if (abs(phid->sensorLastValue[i] - phid->sensorValue[i]) >= phid->sensorChangeTrigger[i]
				|| (phid->sensorLastValue[i] == PUNK_INT))
			{
				FIRE(SensorChange, i, phid->sensorValue[i]);
				phid->sensorLastValue[i] = phid->sensorValue[i];
			}
		}
	}
	for (i = 0; i < phid->phid.attr.ifkit.numInputs; i++)
	{
		if(phid->physicalState[i] != PUNK_BOOL && phid->physicalState[i] != lastInputState[i])
			FIRE(InputChange, i, phid->physicalState[i]);
	}
	for (i = 0; i < phid->phid.attr.ifkit.numOutputs; i++)
	{
		if (phid->outputEchoStates[i] != PUNK_BOOL && phid->outputEchoStates[i] != lastOutputState[i])
			FIRE(OutputChange, i, phid->outputEchoStates[i]);
	}

	return EPHIDGET_OK;
}

//eventsAfterOpen - sends out an event for all valid data, used during attach initialization
CPHIDGETINITEVENTS(InterfaceKit)
	int i = 0;	

	for (i = 0; i < phid->phid.attr.ifkit.numInputs; i++)
	{
		if (phid->physicalState[i] != PUNK_BOOL)
		{
			FIRE(InputChange, i, phid->physicalState[i]);
		}
	}
	for (i = 0; i < phid->phid.attr.ifkit.numOutputs; i++)
	{
		if (phid->outputEchoStates[i] != PUNK_BOOL)
		{
			FIRE(OutputChange, i, phid->outputEchoStates[i]);
		}
	}
	for (i = 0; i < phid->phid.attr.ifkit.numSensors; i++)
	{
		if (phid->sensorValue[i] != PUNK_INT)
		{
			FIRE(SensorChange, i, phid->sensorValue[i]);
		}
	}

	return EPHIDGET_OK;
}

//getPacket - used by write thread to get the next packet to send to device
CGETPACKET(InterfaceKit)
	int i, j, k;

	CPhidgetInterfaceKitHandle phid = (CPhidgetInterfaceKitHandle)phidG;

	TESTPTRS(phid, buf)
	TESTPTR(lenp)

	if (*lenp < phid->phid.outputReportByteLength)
		return EPHIDGET_INVALIDARG;

	ZEROMEM(buf, *lenp)

	CThread_mutex_lock(&phid->phid.outputLock);

	for (i = 0; i < phid->phid.attr.ifkit.numOutputs; i++)
	{
		//Checks for queued up output changes, and propagates them into the output packet
		if(phid->changedOutputs[i]) {
			phid->outputStates[i] = phid->nextOutputStates[i];
			phid->changedOutputs[i] = 0;
			phid->nextOutputStates[i] = PUNK_BOOL;
		}
		//set unknown outputs to false
		if(phid->outputStates[i] == PUNK_BOOL)
			phid->outputStates[i] = PFALSE;
		//set output echoes to output states for non-echoing devices
		if (!phid->fullStateEcho)
			phid->outputEchoStates[i] = phid->outputStates[i];
	}

	//fill in the buffer and length
	switch(phid->phid.deviceIDSpec) {
		case PHIDSPEC_INTERFACEKIT_4_8_8:
			{
				unsigned const char phid488_lookup[8] = {0x00, 0x01, 0x02, 0x03, 0x07, 0x06, 0x05, 0x04};
				buf[0] = phid488_lookup[phid->lastChangedOutput];
				if (phid->outputStates[phid->lastChangedOutput])
					buf[0] |= 0x08;
			}
			break;
		case PHIDSPEC_INTERFACEKIT_0_16_16:
			for (k = 0; k<2; k++)
			{
				for (i = 0, j = 1; i<8; i++, j <<= 1)
				{
					if (phid->outputStates[k * 8 + i])
						buf[k] |= j;
				}
			}
			break;
		case PHIDSPEC_INTERFACEKIT_8_8_8:
		case PHIDSPEC_888_with_lcd:
			for (k = 0, j = 1; k<8; k++, j <<= 1)
			{
				if (phid->outputStates[k])
					buf[0] |= j;
			}

			if(phid->ratiometric == PUNK_BOOL)
				phid->ratiometric = PTRUE;
			buf[3] = (char)phid->ratiometric;

			break;
		case PHIDSPEC_INTERFACEKIT_0_0_4:
		case PHIDSPEC_INTERFACEKIT_0_0_8:
			for (k = 0, j = 1; k < phid->phid.attr.ifkit.numOutputs; k++, j <<= 1)
			{
				if (phid->outputStates[k])
					buf[0] |= j;
			}
			break;
		case PHIDSPEC_INTERFACEKIT_0_5_7:
		case PHIDSPEC_INTERFACEKIT_0_8_8:
			for (i = 0, j=1; i<8; i++, j <<= 1)
			{
				if (phid->outputStates[i])
					buf[0] |= j;
			}
			buf[7] = 0x10;  /* Signal an output */
			break;
		case PHIDSPEC_ROTARY_TOUCH:
		case PHIDSPEC_LINEAR_TOUCH:
			break;
		default:
			return EPHIDGET_UNEXPECTED;
	}

	*lenp = phid->phid.outputReportByteLength;
	
	CThread_mutex_unlock(&phid->phid.outputLock);

	return EPHIDGET_OK;
}

//sendpacket - sends a packet to the device asynchronously, blocking if the 1-packet queue is full
// adds output data to the packet in the queue if possible to combine up to numoutput calls to setoutput into one sent packet
static int CCONV CPhidgetInterfaceKit_sendpacket(CPhidgetInterfaceKitHandle phid,
    unsigned int Index, unsigned int newVal)
{
	int waitReturn;

	CThread_mutex_lock(&phid->phid.writelock);
	phid->lastChangedOutput = Index;

again:
	if (!CPhidget_statusFlagIsSet(&phid->phid, PHIDGET_ATTACHED_FLAG))
	{
		CThread_mutex_unlock(&phid->phid.writelock);
		return EPHIDGET_NOTATTACHED;
	}
	CThread_mutex_lock(&phid->phid.outputLock);
	if (phid->changedOutputs[Index]) {
		if (phid->nextOutputStates[Index] != newVal) {
			CThread_mutex_unlock(&phid->phid.outputLock);
			waitReturn = CThread_wait_on_event(&phid->phid.writtenEvent, 2500);
			switch(waitReturn)
			{
			case WAIT_OBJECT_0:
				break;
			case WAIT_ABANDONED:
				CThread_mutex_unlock(&phid->phid.writelock);
				return EPHIDGET_UNEXPECTED;
			case WAIT_TIMEOUT:
				CThread_mutex_unlock(&phid->phid.writelock);
				return EPHIDGET_TIMEOUT;
			}
			goto again;
		} else {
			CThread_mutex_unlock(&phid->phid.outputLock);
			CThread_mutex_unlock(&phid->phid.writelock);
			return EPHIDGET_OK;
		}
	} else {
		if (phid->outputStates[Index] == newVal) {
			CThread_mutex_unlock(&phid->phid.outputLock);
			CThread_mutex_unlock(&phid->phid.writelock);
			return EPHIDGET_OK;
		}
		phid->changedOutputs[Index] = PTRUE;
		phid->nextOutputStates[Index] = newVal;
		CThread_reset_event(&phid->phid.writtenEvent);
		CThread_mutex_unlock(&phid->phid.outputLock);
		CThread_set_event(&phid->phid.writeAvailableEvent);
	}

	//send output events for devices that don't echo
	if (!(phid->fullStateEcho)) {
		if (phid->outputEchoStates[Index] != (unsigned char)newVal)
		{
			phid->outputEchoStates[Index] = (unsigned char)newVal;
			FIRE(OutputChange, Index, newVal);
		}
	}

	CThread_mutex_unlock(&phid->phid.writelock);

	return EPHIDGET_OK;
}

// === Exported Functions === //

//create and initialize a device structure
CCREATE(InterfaceKit, PHIDDEF_INTERFACEKIT)

//event setup functions
CFHANDLE(InterfaceKit, InputChange, int, int)
CFHANDLE(InterfaceKit, OutputChange, int, int)
CFHANDLE(InterfaceKit, SensorChange, int, int)

CGET(InterfaceKit, InputCount, int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	MASGN(phid.attr.ifkit.numInputs)
}

CGETINDEX(InterfaceKit, InputState, int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	TESTINDEX(phid.attr.ifkit.numInputs)
	TESTMASGN(physicalState[Index], PUNK_BOOL)
	MASGN(physicalState[Index])
}

CGET(InterfaceKit, OutputCount, int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	MASGN(phid.attr.ifkit.numOutputs)
}

CGETINDEX(InterfaceKit, OutputState, int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	TESTINDEX(phid.attr.ifkit.numOutputs)
	TESTMASGN(outputEchoStates[Index], PUNK_BOOL)
	MASGN(outputEchoStates[Index])
}
CSETINDEX(InterfaceKit, OutputState, int)
	TESTPTR(phid)
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	TESTRANGE(PFALSE, PTRUE)
	TESTINDEX(phid.attr.ifkit.numOutputs)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(Output, "%d");
	else
		return CPhidgetInterfaceKit_sendpacket(phid, Index, newVal);
	return EPHIDGET_OK;
}

CGET(InterfaceKit, SensorCount, int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	MASGN(phid.attr.ifkit.numSensors)
}

CGETINDEX(InterfaceKit, SensorValue, int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	TESTINDEX(phid.attr.ifkit.numSensors)
	TESTMASGN(sensorValue[Index], PUNK_INT)
	MASGN(sensorValue[Index])
}

CGETINDEX(InterfaceKit, SensorRawValue, int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	TESTINDEX(phid.attr.ifkit.numSensors)
	TESTMASGN(sensorRawValue[Index], PUNK_INT)
	MASGN(sensorRawValue[Index])
}

CGETINDEX(InterfaceKit, SensorChangeTrigger, int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	TESTINDEX(phid.attr.ifkit.numSensors)
	TESTMASGN(sensorChangeTrigger[Index], PUNK_INT)
	MASGN(sensorChangeTrigger[Index])
}
CSETINDEX(InterfaceKit,SensorChangeTrigger,int)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	TESTINDEX(phid.attr.ifkit.numSensors)
	TESTRANGE(0, 1000)
	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(Trigger, "%d");
	else
		phid->sensorChangeTrigger[Index] = newVal;
	return EPHIDGET_OK;
}

CGET(InterfaceKit, Ratiometric, int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_INTERFACEKIT_8_8_8:
		case PHIDSPEC_888_with_lcd:
			TESTMASGN(ratiometric, PUNK_BOOL)
			MASGN(ratiometric)
		case PHIDSPEC_INTERFACEKIT_0_5_7:
		case PHIDSPEC_INTERFACEKIT_0_8_8:
		case PHIDSPEC_INTERFACEKIT_4_8_8:
		case PHIDSPEC_INTERFACEKIT_0_0_4:
		case PHIDSPEC_INTERFACEKIT_0_0_8:
		case PHIDSPEC_INTERFACEKIT_0_16_16:
			return EPHIDGET_UNSUPPORTED;
		default:
			return EPHIDGET_UNEXPECTED;
	}
}
CSET(InterfaceKit, Ratiometric, int)
	TESTPTR(phid)
	TESTDEVICETYPE(PHIDDEF_INTERFACEKIT)
	TESTATTACHED
	TESTRANGE(PFALSE, PTRUE)

	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_INTERFACEKIT_8_8_8:
		case PHIDSPEC_888_with_lcd:
			if(phid->phid.networkInfo)
				ADDNETWORKKEY(Ratiometric, "%d");
			else
			{
				//this just signals the write thread that a write is available
				CThread_mutex_lock(&phid->phid.writelock);
				CThread_mutex_lock(&phid->phid.outputLock);
				phid->ratiometric = newVal;
				CThread_mutex_unlock(&phid->phid.outputLock);
				CThread_set_event(&phid->phid.writeAvailableEvent);
				CThread_mutex_unlock(&phid->phid.writelock);
			}
			break;
		case PHIDSPEC_INTERFACEKIT_0_5_7:
		case PHIDSPEC_INTERFACEKIT_0_8_8:
		case PHIDSPEC_INTERFACEKIT_4_8_8:
		case PHIDSPEC_INTERFACEKIT_0_0_4:
		case PHIDSPEC_INTERFACEKIT_0_0_8:
		case PHIDSPEC_INTERFACEKIT_0_16_16:
			return EPHIDGET_UNSUPPORTED;
		default:
			return EPHIDGET_UNEXPECTED;
	}

	return EPHIDGET_OK;
}

// === Deprecated Functions === //

CGET(InterfaceKit, NumInputs, int)
	return CPhidgetInterfaceKit_getInputCount(phid, pVal);
}
CGET(InterfaceKit, NumOutputs, int)
	return CPhidgetInterfaceKit_getOutputCount(phid, pVal);
}
CGET(InterfaceKit, NumSensors, int)
	return CPhidgetInterfaceKit_getSensorCount(phid, pVal);
}
