/*
 *  cphidgetdictionary.c
 *  Phidget21
 *
 *  Created by Patrick McNeil on 02/10/06.
 *  Copyright 2006 Phidgets Inc. All rights reserved.
 *
 */

#include "stdafx.h"
#include "cphidgetdictionary.h"
#include "csocket.h"
#include "csocketevents.h"
#include "cphidgetlist.h"
#include "cphidgetmanager.h"

int CPhidgetDictionary_areEqual(void *arg1, void *arg2)
{
	if(arg1 == arg2) return 1;
	return 0;
}

void CPhidgetDictionary_free(void *arg)
{
	CPhidgetDictionaryHandle dict = arg;
	
	if(!dict)
		return;
	
	CThread_mutex_lock(&dict->lock);
	CList_emptyList((CListHandle *)&dict->listeners, PTRUE, CPhidgetDictionaryListener_free);
	CThread_mutex_unlock(&dict->lock);

	CThread_mutex_destroy(&dict->lock);
	free(dict); dict = NULL;
}

int CPhidgetDictionaryListener_areEqual(void *arg1, void *arg2)
{
	if(arg1 == arg2) return 1;
	return 0;
}

void CPhidgetDictionaryListener_free(void *arg)
{
	CPhidgetDictionaryListenerHandle dict = arg;
	
	if(!dict)
		return;
	free(dict); dict = NULL;
}

int CCONV CPhidgetDictionary_create(CPhidgetDictionaryHandle *dict)
{
	CPhidgetDictionaryHandle dicttemp = 0;
	
	TESTPTR(dict)
	if(!(dicttemp = (CPhidgetDictionaryHandle)malloc(sizeof(CPhidgetDictionary))))
		return EPHIDGET_NOMEMORY;
	ZEROMEM(dicttemp, sizeof(CPhidgetDictionary));

	CThread_mutex_init(&dicttemp->lock);

	*dict = dicttemp;
	return EPHIDGET_OK;
}	

int CCONV CPhidgetDictionary_close(CPhidgetDictionaryHandle dict)
{
	int result;
	TESTPTR(dict)

	if(dict->networkInfo)
	{
		CThread_mutex_lock(&serverLock);
		result = unregisterRemoteDictionary(dict);
		CThread_mutex_unlock(&serverLock);
		return result;
	}
	
	return EPHIDGET_OK;
}

int CCONV CPhidgetDictionary_delete(CPhidgetDictionaryHandle dict)
{
	TESTPTR(dict)
	CPhidgetDictionary_free(dict); dict = NULL;
	return EPHIDGET_OK;
}

int CCONV CPhidgetDictionary_getServerID(CPhidgetDictionaryHandle dict, const char **serverID)
{
	return CPhidget_getServerID((CPhidgetHandle)dict, serverID);
}
int CCONV CPhidgetDictionary_getServerAddress(CPhidgetDictionaryHandle dict, const char **address, int *port)
{
	return  CPhidget_getServerAddress((CPhidgetHandle)dict, address, port);
}
int CCONV CPhidgetDictionary_getServerStatus(CPhidgetDictionaryHandle dict, int *status)
{
	return CPhidget_getServerStatus((CPhidgetHandle)dict, status);
}

int CCONV CPhidgetDictionary_set_OnError_Handler(CPhidgetDictionaryHandle dict,
    int(CCONV *fptr)(CPhidgetDictionaryHandle, void *, int, const char *), void *userPtr)
{
	TESTPTR(dict)
	dict->fptrError = fptr;
	dict->fptrErrorptr = userPtr;
	return EPHIDGET_OK;
}

/* Async add - errors returned to a registered error handler */
int CCONV CPhidgetDictionary_addKey(CPhidgetDictionaryHandle dict, const char *key, const char *val, int persistent)
{
	TESTPTR(dict)
	TESTPTRS(key, val)
	if(!dict->networkInfo || !dict->networkInfo->server || !CPhidgetSocketClient_statusFlagIsSet(dict->networkInfo->server, PHIDGETSOCKET_ATTACHED_FLAG))
		return EPHIDGET_NETWORK_NOTCONNECTED;
	pdc_async_set(dict->networkInfo->server->pdcs, key, val, (int)strlen(val), persistent?0:1, internal_async_network_error_handler, dict);
	return EPHIDGET_OK;
}

int CCONV CPhidgetDictionary_removeKey(CPhidgetDictionaryHandle dict, const char *pattern)
{
	int result;
	char err[1024];
	TESTPTRS(dict, pattern)
	if(!dict->networkInfo || !dict->networkInfo->server || !CPhidgetSocketClient_statusFlagIsSet(dict->networkInfo->server, PHIDGETSOCKET_ATTACHED_FLAG))
		return EPHIDGET_NETWORK_NOTCONNECTED;
	CThread_mutex_lock(&dict->networkInfo->server->pdc_lock);
	result = pdc_remove(dict->networkInfo->server->pdcs, pattern, err, sizeof(err));
	CThread_mutex_unlock(&dict->networkInfo->server->pdc_lock);
	if(result == 0) return EPHIDGET_UNEXPECTED;
	return EPHIDGET_OK;
}

int CCONV CPhidgetDictionary_getKey(CPhidgetDictionaryHandle dict, const char *key, const char *val, int valsize)
{
	//int result;
	//char err[1024];
	TESTPTR(dict)
	TESTPTRS(key, val)
	if(!dict->networkInfo || !dict->networkInfo->server || !CPhidgetSocketClient_statusFlagIsSet(dict->networkInfo->server, PHIDGETSOCKET_ATTACHED_FLAG))
		return EPHIDGET_NETWORK_NOTCONNECTED;

	return EPHIDGET_UNSUPPORTED;

/*	CThread_mutex_lock(&dict->networkInfo->server->pdc_lock);
	//result = pdc_get(????);
	CThread_mutex_unlock(&dict->networkInfo->server->pdc_lock);

	if(result == 0) return EPHIDGET_UNEXPECTED;
	return EPHIDGET_OK; */
}

void dict_event_handler(const char *key, const char *val, unsigned int len, pdict_reason_t reason, void *ptr)
{
	CPhidgetDictionaryListenerHandle listener = ptr;
	
	if(!listener) return;
	
	if(listener->fptr)
		listener->fptr(listener->dict, listener->userPtr, key, val, (CPhidgetDictionary_keyChangeReason)reason);
	return;
}

int CCONV CPhidgetDictionary_set_OnServerConnect_Handler(CPhidgetDictionaryHandle dict, int (CCONV *fptr)(CPhidgetDictionaryHandle dict, void *userPtr), void *userPtr)
{
	TESTPTR(dict)
	dict->fptrConnect = fptr; 
	dict->fptrConnectptr = userPtr; 
	return EPHIDGET_OK; 
}
int CCONV CPhidgetDictionary_set_OnServerDisconnect_Handler(CPhidgetDictionaryHandle dict, int (CCONV *fptr)(CPhidgetDictionaryHandle dict, void *userPtr), void *userPtr)
{
	TESTPTR(dict)
	dict->fptrDisconnect = fptr; 
	dict->fptrDisconnectptr = userPtr; 
	return EPHIDGET_OK; 
}

int CCONV CPhidgetDictionary_set_OnKeyChange_Handler(CPhidgetDictionaryHandle dict, CPhidgetDictionaryListenerHandle *dictlistener, const char *pattern, 
	int(CCONV *fptr)(CPhidgetDictionaryHandle dict, void *userPtr, const char *key, const char *val, CPhidgetDictionary_keyChangeReason reason),
	void *userPtr)
{
	CPhidgetDictionaryListenerHandle dict_listener;
	char errdesc[1024];
	int result;

	TESTPTRS(dict, pattern)
	TESTPTR(dictlistener)

	if(!dict->networkInfo || !dict->networkInfo->server || !CPhidgetSocketClient_statusFlagIsSet(dict->networkInfo->server, PHIDGETSOCKET_ATTACHED_FLAG))
		return EPHIDGET_NETWORK_NOTCONNECTED;
		
	if(!(dict_listener = malloc(sizeof(CPhidgetDictionaryListener))))
		return EPHIDGET_NOMEMORY;
	ZEROMEM(dict_listener, sizeof(CPhidgetDictionaryListener));
	
	dict_listener->dict = dict;
	dict_listener->fptr = fptr;
	dict_listener->userPtr = userPtr;
	
	CThread_mutex_lock(&dict->networkInfo->server->pdc_lock);
	if (!(dict_listener->listen_id = pdc_listen(dict->networkInfo->server->pdcs, pattern, dict_event_handler, dict_listener, errdesc, sizeof (errdesc))))
	{
		LOG(PHIDGET_LOG_DEBUG,"pdc_listen: %s", errdesc);
		CThread_mutex_unlock(&dict->networkInfo->server->pdc_lock);
		return EPHIDGET_UNEXPECTED;
	}
	CThread_mutex_unlock(&dict->networkInfo->server->pdc_lock);
	
	CThread_mutex_lock(&dict->lock);
	if((result = CList_addToList((CListHandle *)&dict->listeners, dict_listener, CPhidgetDictionaryListener_areEqual)))
	{
		CThread_mutex_unlock(&dict->lock);
		return result;
	}
	CThread_mutex_unlock(&dict->lock);

	if(dictlistener != NULL)
		*dictlistener = dict_listener;
	
	return EPHIDGET_OK;
}

int CCONV CPhidgetDictionary_remove_OnKeyChange_Handler(CPhidgetDictionaryListenerHandle keylistener)
{
	int result = 0;
	char errdesc[1024];
	CPhidgetDictionaryHandle dict = keylistener->dict;

	TESTPTR(keylistener)

	CThread_mutex_lock(&dict->networkInfo->server->pdc_lock);
	if(!(result = pdc_ignore(dict->networkInfo->server->pdcs, keylistener->listen_id, errdesc, sizeof (errdesc))))
	{
		LOG(PHIDGET_LOG_DEBUG,"pdc_ignore: %s",errdesc);
		CThread_mutex_unlock(&dict->networkInfo->server->pdc_lock);
		return EPHIDGET_UNEXPECTED;
	}
	CThread_mutex_unlock(&dict->networkInfo->server->pdc_lock);

	CThread_mutex_lock(&dict->lock);
	if((result = CList_removeFromList((CListHandle *)&dict->listeners, keylistener, 
		CPhidgetDictionaryListener_areEqual, PTRUE, CPhidgetDictionaryListener_free)))
	{
		CThread_mutex_unlock(&dict->lock);
		return result;
	}
	CThread_mutex_unlock(&dict->lock);

	return EPHIDGET_OK;
}
