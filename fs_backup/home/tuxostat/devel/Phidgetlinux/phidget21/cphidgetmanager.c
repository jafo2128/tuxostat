
#include "stdafx.h"
#include "cphidgetmanager.h"
#include "cphidget.h"
#include "cthread.h"
#include "cusb.h"
#include "cphidgetlist.h"
#include "csocket.h"

/* A list of local Phidget Managers */
CPhidgetManagerListHandle localPhidgetManagers = NULL;
int ActivePhidgetManagers = 0;

/* Protects localPhidgetManagers */
int managerLockInitialized = PFALSE;
CThread_mutex_t managerLock;

int CPhidgetManager_areEqual(void *arg1, void *arg2)
{
	if(arg1 == arg2) return 1;
	return 0;
}

void CPhidgetManager_free(void *arg)
{
	CPhidgetManagerHandle phidm = (CPhidgetManagerHandle)arg;
	
	if(!phidm) return;
	free(phidm); phidm = NULL;
	return;
}

int CCONV CPhidgetManager_set_OnError_Handler(CPhidgetManagerHandle phidm, 
	int(CCONV *fptr)(CPhidgetManagerHandle, void *, int, const char *), void *userPtr)
{
	TESTPTR(phidm)
	phidm->fptrError = fptr;
	phidm->fptrErrorptr = userPtr;
	return EPHIDGET_OK;
}

/* The internal event handlers for PhidgetManager - these should be called directly from the usb functions...
 * these run in the context of the central thread
 * if AddDevice returns EPHIDGET_DUPLICATE, the user callback is not run (good)
 */
int CPhidgetAttachEvent(CPhidgetHandle phid) {
	int result = 0;
	CPhidgetList *trav = 0;
	CPhidgetManagerList *trav2 = 0;
	TESTPTR(phid)
	result = CList_addToList((CListHandle *)&AttachedDevices, phid, CPhidget_areEqual);
	if(result == EPHIDGET_DUPLICATE)
	{
		return EPHIDGET_OK;
	}
	else if(result) 
	{
		return result;
	}
	
	for(trav2 = localPhidgetManagers; trav2; trav2 = trav2->next)
	{
		if (trav2->phidm->fptrAttachChange && trav2->phidm->state == PHIDGETMANAGER_ACTIVE)
			trav2->phidm->fptrAttachChange((CPhidgetHandle)phid, trav2->phidm->fptrAttachChangeptr);
	}

	result = EPHIDGET_OK;

	CThread_mutex_lock(&activeDevicesLock);
	//first look for this specific device
	for (trav=ActiveDevices; trav; trav = trav->next)
	{
		if((trav->phid->serialNumber == phid->serialNumber)
			&& (trav->phid->deviceID == phid->deviceID)
			&& !CPhidget_statusFlagIsSet(trav->phid, PHIDGET_ATTACHED_FLAG))
		{
			result = attachActiveDevice(trav->phid, phid);
			CThread_mutex_unlock(&activeDevicesLock);
			return result;
		}
	}
	//second look for a general device
	for (trav=ActiveDevices; trav; trav = trav->next)
	{
		if(CPhidget_areEqual(trav->phid, phid)
			&& !CPhidget_statusFlagIsSet(trav->phid, PHIDGET_ATTACHED_FLAG))
		{
			result = attachActiveDevice(trav->phid, phid);
			//If one doesn't work, try the next one - this would happen if the first one in opened elsewhere
			if(result != EPHIDGET_OK) continue;
			CThread_mutex_unlock(&activeDevicesLock);
			return result;
		}
	}
	CThread_mutex_unlock(&activeDevicesLock);
	return result;
}

int CPhidgetDetachEvent(CPhidgetHandle phid) {
	int result = 0;
	CPhidgetList *trav = 0;
	CPhidgetManagerList *trav2 = 0;
	CPhidgetHandle travPhid = 0;
	TESTPTR(phid)
	phid->status &= ~PHIDGET_ATTACHED_FLAG;
	phid->status |= PHIDGET_DETACHING_FLAG;
	for(trav2 = localPhidgetManagers; trav2; trav2 = trav2->next)
	{
		if (trav2->phidm->fptrDetachChange && trav2->phidm->state == PHIDGETMANAGER_ACTIVE)
			trav2->phidm->fptrDetachChange((CPhidgetHandle)phid, trav2->phidm->fptrDetachChangeptr);
	}
	phid->status &= ~PHIDGET_DETACHING_FLAG;

	CThread_mutex_lock(&activeDevicesLock);
	for (trav=ActiveDevices; trav; trav = trav->next)
	{	
		if((CPhidget_areExtraEqual(phid, trav->phid) && CPhidget_statusFlagIsSet(trav->phid, PHIDGET_ATTACHED_FLAG)) 
			|| CPhidgetHandle_areEqual(phid, trav->phid))
		{
			CPhidget_setStatusFlag(trav->phid, PHIDGET_DETACHING_FLAG);
			if(trav->phid->specificDevice == 2)
				trav->phid->specificDevice = 0;

			trav->phid->writeStopFlag = PTRUE;
			CThread_set_event(&trav->phid->writeAvailableEvent); //if it's waiting on this event - signal NOW

			result = CUSBCloseHandle(trav->phid);
			CThread_join(&trav->phid->writeThread);
			CThread_join(&trav->phid->readThread);

			//because trav can be freed during the detach call, don't use it in or after the call
			travPhid = trav->phid;
			if (travPhid->fptrDetach)
				travPhid->fptrDetach((CPhidgetHandle)travPhid, travPhid->fptrDetachptr);

			travPhid->deviceIDSpec = 0;
			
			CPhidget_clearStatusFlag(travPhid, PHIDGET_DETACHING_FLAG);
			CPhidget_clearStatusFlag(travPhid, PHIDGET_USB_ERROR_FLAG);
			break;
		}

	}
	CThread_mutex_unlock(&activeDevicesLock);

	result = CList_removeFromList((CListHandle *)&AttachedDevices, phid, CPhidget_areExtraEqual, TRUE, CPhidget_free);

	return result;
}

int CPhidgetManager_poll()
{
	CPhidgetList *curList = 0, *detachList = 0;
	CPhidgetList *trav = 0;
	CPhidgetHandle foundPhidget;
			
	if(!managerLockInitialized)
	{
		CThread_mutex_init(&managerLock);
		managerLockInitialized = PTRUE;
	}
	
	CThread_mutex_lock(&managerLock);
	CThread_mutex_lock(&attachedDevicesLock);
	CUSBBuildList(&curList);


	for (trav=AttachedDevices; trav; trav = trav->next)
	{		
		if(CList_findInList((CListHandle)curList, trav->phid, CPhidget_areExtraEqual, NULL) == EPHIDGET_NOTFOUND)
		{
			CList_addToList((CListHandle *)&detachList, trav->phid, CPhidget_areEqual);
		}
	}
	for (trav=curList; trav; trav = trav->next)
	{
		if(CList_findInList((CListHandle)AttachedDevices, trav->phid, CPhidget_areExtraEqual, NULL) ==
		    EPHIDGET_NOTFOUND)
		{
			CPhidgetAttachEvent(trav->phid);
		}
		
		//if PHIDGET_USB_ERROR_FLAG is set, cycle device through a detach
		//if it's ok, it will re-attach
		//this can't yet handle unexpected timeouts (because we don't have a definitive list of expected timeouts)
		if(CList_findInList((CListHandle)ActiveDevices, trav->phid, CPhidget_areEqual, (void **)&foundPhidget) ==
		    EPHIDGET_OK)
		{		
			if(CPhidget_statusFlagIsSet(foundPhidget, PHIDGET_ATTACHED_FLAG))
			{
				if(CPhidget_statusFlagIsSet(foundPhidget, PHIDGET_USB_ERROR_FLAG))
				{
					LOG(PHIDGET_LOG_WARNING,"PHIDGET_USB_ERROR_FLAG is set - cycling device through a detach");
					CList_addToList((CListHandle *)&detachList, trav->phid, CPhidget_areEqual);
				}
			}
		}
	}
	for (trav=detachList; trav; trav = trav->next)
	{
		CPhidgetDetachEvent(trav->phid);
	}
	CList_emptyList((CListHandle *)&detachList, FALSE, NULL);
	CList_emptyList((CListHandle *)&curList, FALSE, NULL);

	CThread_mutex_unlock(&attachedDevicesLock);
	CThread_mutex_unlock(&managerLock);
	return EPHIDGET_OK;
}

int CCONV CPhidgetManager_create(CPhidgetManagerHandle *phidm)
{
	CPhidgetManagerHandle phidmtemp = 0;
	
	TESTPTR(phidm)
	if(!(phidmtemp = (CPhidgetManagerHandle)malloc(sizeof(CPhidgetManager))))
		return EPHIDGET_NOMEMORY;
	ZEROMEM(phidmtemp, sizeof(CPhidgetManager));
	
	phidmtemp->state = PHIDGETMANAGER_INACTIVE;
	
	if(!managerLockInitialized)
	{
		CThread_mutex_init(&managerLock);
		managerLockInitialized = PTRUE;
	}

	CThread_mutex_init(&phidmtemp->lock);
	
	*phidm = phidmtemp;
	return EPHIDGET_OK;
}

int CCONV CPhidgetManager_close(CPhidgetManagerHandle phidm)
{
	TESTPTR(phidm)

	if(phidm->state == PHIDGETMANAGER_ACTIVE)
	{
		phidm->state = PHIDGETMANAGER_INACTIVE;
		CPhidget_clearStatusFlag((CPhidgetHandle)phidm, PHIDGET_ATTACHED_FLAG);
		if(phidm->networkInfo)
		{
			CThread_mutex_lock(&serverLock);
			unregisterRemoteManager(phidm);
			CThread_mutex_unlock(&serverLock);
		}
		else
		{
			CThread_mutex_lock(&managerLock);
			ActivePhidgetManagers--;
			CList_removeFromList((CListHandle *)&localPhidgetManagers, phidm, CPhidgetManager_areEqual, PFALSE, NULL);
			CThread_mutex_unlock(&managerLock);
		}
	}
	
	//if there are no more active phidgets or managers, wait for the central thread to exit
	if(!ActiveDevices && !ActivePhidgetManagers)
	{
		JoinCentralThread();
	}

	return EPHIDGET_OK;
}

int CCONV CPhidgetManager_delete(CPhidgetManagerHandle phidm)
{
	TESTPTR(phidm)
	if(phidm->networkInfo)
	{
		CList_emptyList((CListHandle *)phidm->AttachedPhidgets, PTRUE, CPhidget_free);
	}
	CPhidgetManager_free(phidm); phidm = NULL;
	return EPHIDGET_OK;
}

int CCONV CPhidgetManager_getServerID(CPhidgetManagerHandle phidm, const char **serverID)
{
	return CPhidget_getServerID((CPhidgetHandle)phidm, serverID);
}
int CCONV CPhidgetManager_getServerAddress(CPhidgetManagerHandle phidm, const char **address, int *port)
{
	return  CPhidget_getServerAddress((CPhidgetHandle)phidm, address, port);
}
int CCONV CPhidgetManager_getServerStatus(CPhidgetManagerHandle phidm, int *status)
{
	return CPhidget_getServerStatus((CPhidgetHandle)phidm, status);
}

int CCONV CPhidgetManager_open(CPhidgetManagerHandle phidm)
{
	int result = EPHIDGET_OK;
	
	TESTPTR(phidm)
	
	if(!phidgetLocksInitialized)
	{
		CThread_mutex_init(&activeDevicesLock);
		CThread_mutex_init(&attachedDevicesLock);
		phidgetLocksInitialized = PTRUE;
	}

	if(phidm->state == PHIDGETMANAGER_INACTIVE)
	{
		CThread_mutex_lock(&managerLock);
	
		CList_addToList((CListHandle *)&localPhidgetManagers, phidm, CPhidgetManager_areEqual);
		
		phidm->state = PHIDGETMANAGER_ACTIVE;
		
		CPhidget_setStatusFlag((CPhidgetHandle)phidm, PHIDGET_ATTACHED_FLAG);

		phidm->AttachedPhidgets = AttachedDevices;
		
		CThread_mutex_lock(&attachedDevicesLock);
		result = CUSBBuildList(&AttachedDevices);
		if(result) 
		{
			CThread_mutex_unlock(&attachedDevicesLock);
			CThread_mutex_unlock(&managerLock);
			return result;
		}
		
		if (phidm->fptrAttachChange)
		{
			CPhidgetList *trav = 0;
			
			for (trav=AttachedDevices; trav; trav = trav->next) {
				phidm->fptrAttachChange((CPhidgetHandle)(trav->phid), phidm->fptrAttachChangeptr);
			}
		}
		ActivePhidgetManagers++;
		
		CThread_mutex_unlock(&attachedDevicesLock);
		CThread_mutex_unlock(&managerLock);
		
		result = StartCentralThread();
	}
	return result;
}

int CCONV CPhidgetManager_set_OnAttach_Handler(CPhidgetManagerHandle phidm, int (CCONV *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr)
{
	TESTPTR(phidm)
	phidm->fptrAttachChange = fptr; 
	phidm->fptrAttachChangeptr = userPtr; 
	return EPHIDGET_OK; 
}
int CCONV CPhidgetManager_set_OnDetach_Handler(CPhidgetManagerHandle phidm, int (CCONV *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr)
{
	TESTPTR(phidm)
	phidm->fptrDetachChange = fptr; 
	phidm->fptrDetachChangeptr = userPtr; 
	return EPHIDGET_OK; 
}
int CCONV CPhidgetManager_set_OnServerConnect_Handler(CPhidgetManagerHandle phidm, int (CCONV *fptr)(CPhidgetManagerHandle phidm, void *userPtr), void *userPtr)
{
	TESTPTR(phidm)
	phidm->fptrConnect = fptr; 
	phidm->fptrConnectptr = userPtr; 
	return EPHIDGET_OK; 
}
int CCONV CPhidgetManager_set_OnServerDisconnect_Handler(CPhidgetManagerHandle phidm, int (CCONV *fptr)(CPhidgetManagerHandle phidm, void *userPtr), void *userPtr)
{
	TESTPTR(phidm)
	phidm->fptrDisconnect = fptr; 
	phidm->fptrDisconnectptr = userPtr; 
	return EPHIDGET_OK; 
}

int CCONV CPhidgetManager_getAttachedDevices(CPhidgetManagerHandle phidm, CPhidgetHandle *phidArray[],
    int *count)
{
	CPhidgetList *trav = 0;
	int i = 0;

	TESTPTRS(phidArray, count)
	TESTPTR(phidm)

	*count = 0;
	if(phidm->networkInfo)
	{		
		for (trav=phidm->AttachedPhidgets; trav; trav = trav->next)
			(*count)++;
		
		*phidArray = (CPhidgetHandle *)malloc(sizeof(**phidArray) * *count);
		if (!*phidArray)
			return EPHIDGET_NOMEMORY;
		ZEROMEM(*phidArray, sizeof(**phidArray) * *count);

		for (trav=phidm->AttachedPhidgets, i=0; trav; trav = trav->next, i++)
			(*phidArray)[i] = trav->phid;
	}
	else
	{
		CThread_mutex_lock(&attachedDevicesLock);
		for (trav=AttachedDevices; trav; trav = trav->next)
			(*count)++;
		
		*phidArray = (CPhidgetHandle *)malloc(sizeof(**phidArray) * *count);
		if (!*phidArray)
		{
			CThread_mutex_unlock(&attachedDevicesLock);
			return EPHIDGET_NOMEMORY;
		}
		ZEROMEM(*phidArray, sizeof(**phidArray) * *count);

		for (trav=AttachedDevices, i=0; trav; trav = trav->next, i++)
			(*phidArray)[i] = trav->phid;

		CThread_mutex_unlock(&attachedDevicesLock);
	}
	return EPHIDGET_OK; 
}
