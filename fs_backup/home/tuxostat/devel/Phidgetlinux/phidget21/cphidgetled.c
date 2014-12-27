#include "stdafx.h"
#include "cphidgetled.h"
#include "cusb.h"
#include "csocket.h"
#include "cthread.h"

// === Internal Functions === //

//clearVars - sets all device variables to unknown state
CPHIDGETCLEARVARS(LED)
	int i = 0;

	phid->changeRequests=PUNK_BOOL;

	for(i=0;i<LED_MAXLEDS;i++)
	{
		phid->changedLED_Power[i] = PUNK_BOOL;
		phid->LED_Power[i] = PUNK_INT;
		phid->nextLED_Power[i] = PUNK_INT;
	}

	return EPHIDGET_OK;
}

//initAfterOpen - sets up the initial state of an object, reading in packets from the device if needed
//				  used during attach initialization - on every attach
CPHIDGETINIT(LED)
	int i = 0;

	TESTPTR(phid);

	//set data arrays to unknown
	for(i=0;i<phid->phid.attr.led.numLEDs;i++)
	{
		phid->changedLED_Power[i] = PFALSE;
		phid->LED_Power[i] = PUNK_INT;
		phid->nextLED_Power[i] = PUNK_INT;
	}
	phid->changeRequests=0;

	return EPHIDGET_OK;
}

//dataInput - parses device packets - not used
CPHIDGETDATA(LED)
	phid = 0;
	return EPHIDGET_UNEXPECTED;
}

//eventsAfterOpen - sends out an event for all valid data, used during attach initialization - not used
CPHIDGETINITEVENTS(LED)
	phid = 0;
	return EPHIDGET_UNEXPECTED;
}

//getPacket - used by write thread to get the next packet to send to device
CGETPACKET(LED)
	int i = 0;
	int numLeds = 0;

	CPhidgetLEDHandle phid = (CPhidgetLEDHandle)phidG;

	TESTPTRS(phid, buf)
	TESTPTR(lenp)

	if (*lenp < phid->phid.outputReportByteLength)
		return EPHIDGET_INVALIDARG;

	CThread_mutex_lock(&phid->phid.outputLock);
	
	switch(phid->phid.deviceIDSpec)
	{
		case PHIDSPEC_LED:
			if ((phid->phid.deviceVersion >= 100) && (phid->phid.deviceVersion < 300))
			{

				//construct the packet, with up to 4 LED sets
				for (i = 0; i < phid->phid.attr.led.numLEDs; i++)
				{
					if (phid->changedLED_Power[i] && numLeds < 4) {
						phid->LED_Power[i] = phid->nextLED_Power[i];
						phid->changedLED_Power[i] = PFALSE;
						phid->nextLED_Power[i] = PUNK_INT;
						buf[numLeds*2] = i;
						//0-100 -> 0-63
						buf[numLeds*2+1] = (unsigned char)round((phid->LED_Power[i] / 100.0) * 63.0);
						numLeds++;
						phid->changeRequests--;
					}
				}

				//fill up any remaining buffer space with valid data - sending 0's will mess things up
				for(numLeds=numLeds;numLeds<4;numLeds++)
				{
					buf[numLeds*2] = buf[(numLeds-1)*2];
					buf[numLeds*2+1] = buf[(numLeds-1)*2+1];
				}
			}
			else
				return EPHIDGET_UNEXPECTED;
			break;
		default:
			return EPHIDGET_UNEXPECTED;
	}
	
	//if there are still pending sets, signal the event again (which will tell write thread to call this funciton again)
	if(phid->changeRequests)
		CThread_set_event(&phid->phid.writeAvailableEvent);

	*lenp = phid->phid.outputReportByteLength;

	CThread_mutex_unlock(&phid->phid.outputLock);

	return EPHIDGET_OK;
}

//sendpacket - sends a packet to the device asynchronously, blocking if the 1-packet queue is full
//	-every LED has its own 1 state mini-queue
static int CCONV CPhidgetLED_sendpacket(CPhidgetLEDHandle phid,
    unsigned int index, unsigned int power)
{
	int waitReturn;
	CThread_mutex_lock(&phid->phid.writelock);
again:
	if (!CPhidget_statusFlagIsSet(&phid->phid, PHIDGET_ATTACHED_FLAG))
	{
		CThread_mutex_unlock(&phid->phid.writelock);
		return EPHIDGET_NOTATTACHED;
	}
	CThread_mutex_lock(&phid->phid.outputLock);
	//if we have already requested a change on this LED
	if (phid->changedLED_Power[index]) {
		//and it was different then this time
		if (phid->nextLED_Power[index] != power) {
			CThread_mutex_unlock(&phid->phid.outputLock);
			//then wait for it to get written
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
			//and try again
			goto again;
		} else {
			CThread_mutex_unlock(&phid->phid.outputLock);
			CThread_mutex_unlock(&phid->phid.writelock);
			return EPHIDGET_OK;
		}
	//otherwise
	} else {
		//if it's different then current, queue it up
		if (phid->LED_Power[index] != power) {
			phid->changeRequests++;
			phid->changedLED_Power[index] = PTRUE;
			phid->nextLED_Power[index] = power;
			CThread_reset_event(&phid->phid.writtenEvent);
			CThread_mutex_unlock(&phid->phid.outputLock);
			CThread_set_event(&phid->phid.writeAvailableEvent);
		}
		//if it's the same, just return
		else
		{
			CThread_mutex_unlock(&phid->phid.outputLock);
			CThread_mutex_unlock(&phid->phid.writelock);
			return EPHIDGET_OK;
		}
	}
	CThread_mutex_unlock(&phid->phid.writelock);
	return EPHIDGET_OK;
}

// === Exported Functions === //

//create and initialize a device structure
CCREATE(LED, PHIDDEF_LED)

CGET(LED,LEDCount,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_LED)
	TESTATTACHED
	MASGN(phid.attr.led.numLEDs)
}

CGETINDEX(LED,DiscreteLED,int)
	TESTPTRS(phid,pVal) 
	TESTDEVICETYPE(PHIDDEF_LED)
	TESTATTACHED
	TESTINDEX(phid.attr.led.numLEDs)
	TESTMASGN(LED_Power[Index], PUNK_INT)
	MASGN(LED_Power[Index])
}
CSETINDEX(LED,DiscreteLED,int)
	TESTPTR(phid) 
	TESTDEVICETYPE(PHIDDEF_LED)
	TESTATTACHED
	TESTINDEX(phid.attr.led.numLEDs)
	TESTRANGE(0, 100)

	if(phid->phid.networkInfo)
		ADDNETWORKKEYINDEXED(Brightness, "%d");
	else
	{
		return CPhidgetLED_sendpacket(phid, Index, newVal);
	}
	return EPHIDGET_OK;
}

// === Deprecated Functions === //

CGET(LED,NumLEDs,int)
	return CPhidgetLED_getLEDCount(phid, pVal);
}
