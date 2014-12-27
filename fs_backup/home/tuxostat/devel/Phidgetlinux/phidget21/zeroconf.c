#include "stdafx.h"
#include "csocket.h"
#include "csocketevents.h"
#include "cphidgetlist.h"
#include "cphidgetmanager.h"
#include "cphidgetdictionary.h"
#include "zeroconf.h"
#include "dns_sd.h"

#ifdef ZEROCONF_RUNTIME_LINKING
	//function prototypes for run-time loaded library
	typedef DNSServiceErrorType (DNSSD_API * DNSServiceRegisterType) 
		(DNSServiceRef,DNSServiceFlags,uint32_t,const char *,
		const char *,const char *,const char *,uint16_t,uint16_t,
		const void *,DNSServiceRegisterReply,void *);
	typedef DNSServiceErrorType (DNSSD_API * DNSServiceProcessResultType) (DNSServiceRef);
	typedef void (DNSSD_API * DNSServiceRefDeallocateType) (DNSServiceRef);
	typedef DNSServiceErrorType (DNSSD_API * DNSServiceAddRecordType)
		(DNSServiceRef, DNSRecordRef *, DNSServiceFlags, 
		uint16_t, uint16_t, const void *, uint32_t);
	typedef DNSServiceErrorType (DNSSD_API * DNSServiceUpdateRecordType)
		(DNSServiceRef, DNSRecordRef, DNSServiceFlags, 
		uint16_t, const void *, uint32_t);
	typedef DNSServiceErrorType (DNSSD_API * DNSServiceRemoveRecordType)
		(DNSServiceRef, DNSRecordRef, DNSServiceFlags);
	typedef DNSServiceErrorType (DNSSD_API * DNSServiceBrowseType)
		(DNSServiceRef *, DNSServiceFlags, uint32_t, const char *,
		const char *, DNSServiceBrowseReply, void *);
	typedef DNSServiceErrorType (DNSSD_API * DNSServiceResolveType)
		(DNSServiceRef *, DNSServiceFlags, uint32_t, const char *,
		const char *, const char *, DNSServiceResolveReply, 
		void *context);
	//typedef const void * (DNSSD_API * TXTRecordGetValuePtrType)
	//	(uint16_t, const void*, const char *, uint8_t *);
	typedef DNSServiceErrorType (DNSSD_API * DNSServiceQueryRecordType)
		(DNSServiceRef *, DNSServiceFlags, uint32_t, const char *,
		uint16_t, uint16_t, DNSServiceQueryRecordReply, void *context);
	typedef int (DNSSD_API * DNSServiceConstructFullNameType)
		(char *, const char *, const char *, const char *);
#else
	#define DNSServiceRegisterPtr DNSServiceRegister
	#define DNSServiceProcessResultPtr DNSServiceProcessResult
	#define DNSServiceRefDeallocatePtr DNSServiceRefDeallocate
	#define DNSServiceAddRecordPtr DNSServiceAddRecord
	#define DNSServiceUpdateRecordPtr DNSServiceUpdateRecord
	#define DNSServiceRemoveRecordPtr DNSServiceRemoveRecord
	#define DNSServiceBrowsePtr DNSServiceBrowse
	#define DNSServiceResolvePtr DNSServiceResolve
	//#define TXTRecordGetValuePtrPtr TXTRecordGetValuePtr
	#define DNSServiceQueryRecordPtr DNSServiceQueryRecord
	#define DNSServiceConstructFullNamePtr DNSServiceConstructFullName
#endif

/* 
 * TXT record version - this should be 1 for a long time
 *  - only need to change if we really change the TXT record format
 */
const char *dnssd_txt_ver = "1";

int Dns_sdInitialized = FALSE; 
DNSServiceRef zeroconf_browse_ws_ref  = NULL;
DNSServiceRef zeroconf_browse_phidget_ref  = NULL;

pthread_t dns_thread_ws, dns_thread_phid;

#ifdef ZEROCONF_RUNTIME_LINKING
#ifdef _WINDOWS
HMODULE dllHandle = NULL;
//DNS_SD functions
DNSServiceRegisterType DNSServiceRegisterPtr = NULL;
DNSServiceProcessResultType DNSServiceProcessResultPtr = NULL;
DNSServiceRefDeallocateType DNSServiceRefDeallocatePtr = NULL;
DNSServiceAddRecordType DNSServiceAddRecordPtr = NULL;
DNSServiceUpdateRecordType DNSServiceUpdateRecordPtr = NULL;
DNSServiceRemoveRecordType DNSServiceRemoveRecordPtr = NULL;
DNSServiceBrowseType DNSServiceBrowsePtr = NULL;
DNSServiceResolveType DNSServiceResolvePtr = NULL;
//TXTRecordGetValuePtrType TXTRecordGetValuePtrPtr = NULL;
DNSServiceQueryRecordType DNSServiceQueryRecordPtr = NULL;
DNSServiceConstructFullNameType DNSServiceConstructFullNamePtr = NULL;

#elif _LINUX
#endif
#endif

	
static uint8_t *InternalTXTRecordSearch
	(
	uint16_t         txtLen,
	const void       *txtRecord,
	const char       *key,
	unsigned long    *keylen
	)
{
	uint8_t *p = (uint8_t*)txtRecord;
	uint8_t *e = p + txtLen;
	*keylen = (unsigned long) strlen(key);
	while (p<e)
		{
		uint8_t *x = p;
		p += 1 + p[0];
		if (p <= e && *keylen <= x[0] && !strncmp(key, (char*)x+1, *keylen))
			if (*keylen == x[0] || x[1+*keylen] == '=') return(x);
		}
	return(NULL);
}

const void * TXTRecordGetValuePtrPtr
	(
	uint16_t         txtLen,
	const void       *txtRecord,
	const char       *key,
	uint8_t          *valueLen
	)
{
	unsigned long keylen;
	uint8_t *item = InternalTXTRecordSearch(txtLen, txtRecord, key, &keylen);
	if (!item || item[0] <= keylen) return(NULL);	// If key not found, or found with no value, return NULL
	*valueLen = (uint8_t)(item[0] - (keylen + 1));
	return (item + 1 + keylen + 1);
}


int dns_callback_thread(void * ref)
{
    while (DNSServiceProcessResultPtr(ref) == kDNSServiceErr_NoError)
        continue;
	return EPHIDGET_OK;
}

void PhidFromTXT(CPhidgetHandle phid, uint16_t txtLen, const char *txtRecord)
{
	int i = 0;
	char *type = NULL;
	char *serial = NULL;
	char *version = NULL;
	char *name = NULL;

	uint8_t valLen = 0;
	const char *valPtr = NULL;

	//Serial Number
	if(!(valPtr = TXTRecordGetValuePtrPtr(txtLen, txtRecord, "serial", &valLen))) return;
	if(!(serial = malloc(valLen+1))) return;
	ZEROMEM(serial, valLen+1);
	memcpy(serial, valPtr, valLen);
	phid->serialNumber = strtol(serial, NULL, 10);
	phid->specificDevice = PTRUE;
	free(serial);

	//name
	if(!(valPtr = TXTRecordGetValuePtrPtr(txtLen, txtRecord, "name", &valLen))) return;
	if(!(name = malloc(valLen+1))) return;
	ZEROMEM(name, valLen+1);
	memcpy(name, valPtr, valLen);
	for(i = 0;i<PHIDGET_DEVICE_COUNT;i++)
	{
		if(!strcmp(name, Phid_DeviceSpecificName[i]))
		{
			phid->deviceIDSpec = i;
			phid->attr = Phid_Device_Def[i].pdd_attr;
			break;
		}
	}
	free(name);

	//type
	if(!(valPtr = TXTRecordGetValuePtrPtr(txtLen, txtRecord, "type", &valLen))) return;
	if(!(type = malloc(valLen+1))) return;
	ZEROMEM(type, valLen+1);
	memcpy(type, valPtr, valLen);
	phid->deviceID = phidget_type_to_id(type);
	phid->deviceType = Phid_DeviceName[phid->deviceID];
	free(type);

	//version
	if(!(valPtr = TXTRecordGetValuePtrPtr(txtLen, txtRecord, "version", &valLen))) return;
	if(!(version = malloc(valLen+1))) return;
	ZEROMEM(version, valLen+1);
	memcpy(version, valPtr, valLen);
	phid->deviceVersion = strtol(version, NULL, 10);
	free(version);

	//label
	if(!(valPtr = TXTRecordGetValuePtrPtr(txtLen, txtRecord, "label", &valLen))) return;
	if(valLen > 10) valLen = 10;
	memcpy(phid->label, valPtr, valLen);
	phid->label[valLen] = '\0';

	//server_id
	if(!(valPtr = TXTRecordGetValuePtrPtr(txtLen, txtRecord, "server_id", &valLen))) return;
	if(!(phid->networkInfo->zeroconf_server_id = malloc(valLen+1))) return;
	ZEROMEM(phid->networkInfo->zeroconf_server_id, valLen+1);
	memcpy(phid->networkInfo->zeroconf_server_id, valPtr, valLen);

	phid->networkInfo->mdns = PTRUE;

}

void DNSServiceQueryRecord_Phidget_CallBack
    (
    DNSServiceRef                       DNSServiceRef,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    DNSServiceErrorType                 errorCode,
    const char                          *fullname,
    uint16_t                            rrtype,
    uint16_t                            rrclass,
    uint16_t                            rdlen,
    const void                          *rdata,
    uint32_t                            ttl,
    void                                *context
    )
{
	if (errorCode != kDNSServiceErr_NoError)
		LOG(PHIDGET_LOG_ERROR, "DNSServiceQueryRecord_Phidget_CallBack returned error: %d\n", errorCode);
	else
	{
		CPhidgetHandle phid = (CPhidgetHandle)context;
		LOG(PHIDGET_LOG_INFO, "DNSServiceQueryRecord_Phidget_CallBack: %s",fullname);

		PhidFromTXT(phid, rdlen, rdata);
	}
}

void DNSServiceResolve_CallBack(DNSServiceRef sdRef,
				DNSServiceFlags                     flags,
				uint32_t                            interfaceIndex,
				DNSServiceErrorType                 errorCode,
				const char                          *fullname,
				const char                          *hosttarget,
				uint16_t                            port,
				uint16_t                            txtLen,
				const char                          *txtRecord,
				void                                *context)
{	
	if (errorCode != kDNSServiceErr_NoError)
		LOG(PHIDGET_LOG_ERROR, "DNSServiceResolve_CallBack returned error: %d\n", errorCode);
	else
	{
		CPhidgetRemoteHandle networkInfo = (CPhidgetRemoteHandle)context;
		
		LOG(PHIDGET_LOG_INFO, "DNSServiceResolve_CallBack: %s",fullname);

		networkInfo->zeroconf_host = strdup(hosttarget);
		networkInfo->zeroconf_port = malloc(10);
		snprintf(networkInfo->zeroconf_port, 10, "%d", ntohs(port));
	}
}

void DNSServiceQueryRecord_ws_CallBack
    (
    DNSServiceRef                       DNSServiceRef,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    DNSServiceErrorType                 errorCode,
    const char                          *fullname,
    uint16_t                            rrtype,
    uint16_t                            rrclass,
    uint16_t                            rdlen,
    const void                          *rdata,
    uint32_t                            ttl,
    void                                *context
    )
{
	if (errorCode != kDNSServiceErr_NoError)
		LOG(PHIDGET_LOG_ERROR, "DNSServiceQueryRecord_ws_CallBack returned error: %d\n", errorCode);
	else
	{
		uint8_t valLen = 0;
		const char *valPtr = NULL;
		CPhidgetRemoteHandle networkInfo = (CPhidgetRemoteHandle)context;
		LOG(PHIDGET_LOG_INFO, "DNSServiceQueryRecord_ws_CallBack: %s",fullname);

		//server_id
		if(!(valPtr = TXTRecordGetValuePtrPtr(rdlen, rdata, "server_id", &valLen))) return;
		if(!(networkInfo->zeroconf_server_id = malloc(valLen+1))) return;
		ZEROMEM(networkInfo->zeroconf_server_id, valLen+1);
		memcpy(networkInfo->zeroconf_server_id, valPtr, valLen);
	}
}

void DNSServiceBrowse_Phidget_CallBack(DNSServiceRef service,
                DNSServiceFlags flags,
                uint32_t interfaceIndex,
                DNSServiceErrorType errorCode,
                const char * name,
                const char * type,
                const char * domain,
                void * context)
{
	if (errorCode != kDNSServiceErr_NoError)
		LOG(PHIDGET_LOG_ERROR, "Dns_sd_BrowseCallBack returned error: %d\n", errorCode);
	else
	{    
		DNSServiceErrorType error;
		DNSServiceRef  serviceRef;
		char fullname[kDNSServiceMaxDomainName];

		CPhidgetHandle phid;
		CPhidgetManagerList *trav;

		DNSServiceConstructFullNamePtr(fullname, name, type, domain);

		LOG(PHIDGET_LOG_INFO, "DNSServiceBrowse_Phidget_CallBack: %s",name);

		if((CPhidget_create(&phid))) return;
		if((CPhidgetRemote_create(&phid->networkInfo))) return;

		phid->networkInfo->zeroconf_name = strdup(name);
		phid->networkInfo->zeroconf_type = strdup(type);
		phid->networkInfo->zeroconf_domain = strdup(domain);

		if(flags & kDNSServiceFlagsAdd)
		{
			error = DNSServiceQueryRecordPtr(&serviceRef, 0, interfaceIndex, fullname,
				kDNSServiceType_TXT, kDNSServiceClass_IN, DNSServiceQueryRecord_Phidget_CallBack, phid);
			if (error == kDNSServiceErr_NoError)
			{
				DNSServiceProcessResultPtr(serviceRef);
				DNSServiceRefDeallocatePtr(serviceRef);
				CThread_mutex_lock(&zeroconfPhidgetsLock);
				CThread_mutex_lock(&activeRemoteManagersLock);
				
				CPhidget_setStatusFlag(phid, PHIDGET_ATTACHED_FLAG);
				if(CList_findInList((CListHandle)zeroconfPhidgets, phid, CPhidget_areEqual, NULL))
				{
					CList_addToList((CListHandle *)&zeroconfPhidgets, phid, CPhidget_areEqual);
					//managers
					for (trav=activeRemoteManagers; trav; trav = trav->next)
					{
						if(trav->phidm->networkInfo->requested_address==NULL
							&& (trav->phidm->networkInfo->requested_serverID == NULL || !strcmp(trav->phidm->networkInfo->requested_serverID,phid->networkInfo->zeroconf_server_id)))
						{
							if (trav->phidm->fptrAttachChange && trav->phidm->state == PHIDGETMANAGER_ACTIVE)
								trav->phidm->fptrAttachChange((CPhidgetHandle)phid, trav->phidm->fptrAttachChangeptr);
						}
					}
				}
				CThread_mutex_unlock(&activeRemoteManagersLock);
				CThread_mutex_unlock(&zeroconfPhidgetsLock);
			}
			else
				LOG(PHIDGET_LOG_ERROR, "DNSServiceQueryRecordPtr returned error: %d\n", error);
		}
		else
		{
			//have to fill in phid manually from just the name
			int i;
			char *name_copy = strdup(name);
			for(i=0;i<strlen(name_copy);i++)
				if(name_copy[i] == '(') break;
			if(i<=1) return;
			name_copy[strlen(name_copy)-1]='\0';
			name_copy[i-1] = '\0';
			phid->serialNumber = strtol(name_copy+i+1, NULL, 10);
			phid->specificDevice = PTRUE;
			for(i = 0;i<PHIDGET_DEVICE_COUNT;i++)
				if(!strcmp(name_copy, Phid_DeviceSpecificName[i])) break;
			phid->deviceIDSpec = i;
			phid->attr = Phid_Device_Def[i].pdd_attr;
			phid->deviceID = Phid_Device_Def[i].pdd_did;
			phid->deviceType = Phid_DeviceName[phid->deviceID];
			phid->networkInfo->mdns = PTRUE;

			CThread_mutex_lock(&zeroconfPhidgetsLock);
			CThread_mutex_lock(&activeRemoteManagersLock);

			CPhidget_clearStatusFlag(phid, PHIDGET_ATTACHED_FLAG);
			CPhidget_setStatusFlag(phid, PHIDGET_DETACHING_FLAG);
			if(!CList_findInList((CListHandle)zeroconfPhidgets, phid, CPhidget_areEqual, NULL))
			{
				CList_removeFromList((CListHandle *)&zeroconfPhidgets, phid, CPhidget_areExtraEqual, TRUE, CPhidget_free);
				//managers
				for (trav=activeRemoteManagers; trav; trav = trav->next)
				{
					if(trav->phidm->networkInfo->requested_address==NULL
						&& (trav->phidm->networkInfo->requested_serverID == NULL || !strcmp(trav->phidm->networkInfo->requested_serverID,phid->networkInfo->zeroconf_server_id)))
					{
						if (trav->phidm->fptrDetachChange && trav->phidm->state == PHIDGETMANAGER_ACTIVE)
							trav->phidm->fptrDetachChange((CPhidgetHandle)phid, trav->phidm->fptrDetachChangeptr);
					}
				}
				CPhidget_clearStatusFlag(phid, PHIDGET_DETACHING_FLAG);
				CPhidget_free(phid);
			}
			CThread_mutex_unlock(&activeRemoteManagersLock);
			CThread_mutex_unlock(&zeroconfPhidgetsLock);
			free(name_copy);
		}
	}
}

void DNSServiceBrowse_ws_CallBack(DNSServiceRef service,
                DNSServiceFlags flags,
                uint32_t interfaceIndex,
                DNSServiceErrorType errorCode,
                const char * name,
                const char * type,
                const char * domain,
                void * context)
{
	if (errorCode != kDNSServiceErr_NoError)
		LOG(PHIDGET_LOG_ERROR, "DNSServiceBrowse_ws_CallBack returned error: %d\n", errorCode);
	else
	{
		char fullname[kDNSServiceMaxDomainName];

		CPhidgetRemoteHandle networkInfo;
		if((CPhidgetRemote_create(&networkInfo))) return;

		networkInfo->zeroconf_name = strdup(name);
		networkInfo->zeroconf_server_id = strdup(name);
		networkInfo->zeroconf_type = strdup(type);
		networkInfo->zeroconf_domain = strdup(domain);

		DNSServiceConstructFullNamePtr(fullname, name, type, domain);

		LOG(PHIDGET_LOG_INFO, "DNSServiceBrowse_ws_CallBack: %s",name);

		CThread_mutex_lock(&zeroconfServersLock);
		if(flags & kDNSServiceFlagsAdd)
		{
			CList_addToList((CListHandle *)&zeroconfServers, networkInfo, CPhidgetRemote_areEqual);
		}
		else
		{
			CList_removeFromList((CListHandle *)&zeroconfServers, networkInfo, CPhidgetRemote_areEqual, TRUE, CPhidgetRemote_free);
		}
		CThread_mutex_unlock(&zeroconfServersLock);
	}
}

int getZeroconfHostPort(CPhidgetRemoteHandle networkInfo)
{
	DNSServiceErrorType error;
	DNSServiceRef  serviceRef;
	error = DNSServiceResolvePtr(&serviceRef,
							   0,  // no flags
							   0,  // all network interfaces
							   networkInfo->zeroconf_name, //name
							   networkInfo->zeroconf_type, // service type
							   networkInfo->zeroconf_domain, //domain
							   DNSServiceResolve_CallBack,
							   networkInfo);     // no context
	DNSServiceProcessResultPtr(serviceRef);
	DNSServiceRefDeallocatePtr(serviceRef);
	return EPHIDGET_OK;
}

int InitializeZeroconf()
{
	DNSServiceErrorType error;
	if(!Dns_sdInitialized)
	{
	#ifdef ZEROCONF_RUNTIME_LINKING

	#ifdef _WINDOWS
		if(!(dllHandle = LoadLibrary(L"dnssd.dll")))
		{
			DWORD error = GetLastError();
			switch(error)
			{
			case ERROR_MOD_NOT_FOUND:
				LOG(PHIDGET_LOG_DEBUG,"LoadLibrary failed - module could not be found");
				break;
			default:
				LOG(PHIDGET_LOG_DEBUG,"LoadLibrary failed with error code: %d", error);
			}
			return EPHIDGET_UNEXPECTED;
		}

	   // If the handle is valid, try to get the function address. 
	   if (NULL != dllHandle) 
	   { 
		  //Get pointers to our functions using GetProcAddress:
		  DNSServiceRegisterPtr = (DNSServiceRegisterType)GetProcAddress(dllHandle, "DNSServiceRegister");
		  DNSServiceProcessResultPtr = (DNSServiceProcessResultType)GetProcAddress(dllHandle, "DNSServiceProcessResult");
		  DNSServiceRefDeallocatePtr = (DNSServiceRefDeallocateType)GetProcAddress(dllHandle, "DNSServiceRefDeallocate");
		  DNSServiceAddRecordPtr = (DNSServiceAddRecordType)GetProcAddress(dllHandle, "DNSServiceAddRecord");
		  DNSServiceUpdateRecordPtr = (DNSServiceUpdateRecordType)GetProcAddress(dllHandle, "DNSServiceUpdateRecord");
		  DNSServiceRemoveRecordPtr = (DNSServiceRemoveRecordType)GetProcAddress(dllHandle, "DNSServiceRemoveRecord");
		  DNSServiceBrowsePtr = (DNSServiceBrowseType)GetProcAddress(dllHandle, "DNSServiceBrowse");
		  DNSServiceResolvePtr = (DNSServiceResolveType)GetProcAddress(dllHandle, "DNSServiceResolve");
		  //TXTRecordGetValuePtrPtr = (TXTRecordGetValuePtrType)GetProcAddress(dllHandle, "TXTRecordGetValuePtr");
		  DNSServiceQueryRecordPtr = (DNSServiceQueryRecordType)GetProcAddress(dllHandle, "DNSServiceQueryRecord");
		  DNSServiceConstructFullNamePtr = (DNSServiceConstructFullNameType)GetProcAddress(dllHandle, "DNSServiceConstructFullName");

		  Dns_sdInitialized = (
			  NULL != DNSServiceRegisterPtr && 
			  NULL != DNSServiceProcessResultPtr &&
			  NULL != DNSServiceRefDeallocatePtr &&
			  NULL != DNSServiceAddRecordPtr &&
			  NULL != DNSServiceUpdateRecordPtr &&
			  NULL != DNSServiceRemoveRecordPtr &&
			  //NULL != TXTRecordGetValuePtrPtr &&
			  NULL != DNSServiceQueryRecordPtr &&
			  NULL != DNSServiceConstructFullNamePtr);
	   }
	#elif _LINUX
	#endif
	   if(!Dns_sdInitialized)
	   {
			LOG(PHIDGET_LOG_DEBUG,"InitializeZeroconf failed somehow...");
			return EPHIDGET_UNEXPECTED;
	   }
	#else
		Dns_sdInitialized = TRUE;
	#endif

		error = DNSServiceBrowsePtr(&zeroconf_browse_ws_ref,
                            0,                // no flags
                            0,                // all network interfaces
                            "_phidget_ws._tcp",     // service type
                            "",               // default domains
                            DNSServiceBrowse_ws_CallBack, // call back function
                            NULL);            // no context
		if (error == kDNSServiceErr_NoError)
        {
			pthread_create(&dns_thread_ws, NULL, (void *(*)(void *))dns_callback_thread,zeroconf_browse_ws_ref);
        }
		error = DNSServiceBrowsePtr(&zeroconf_browse_phidget_ref,
                            0,                // no flags
                            0,                // all network interfaces
                            "_phidget._tcp",     // service type
                            "",               // default domains
                            DNSServiceBrowse_Phidget_CallBack, // call back function
                            NULL);            // no context
		if (error == kDNSServiceErr_NoError)
        {
			pthread_create(&dns_thread_phid, NULL, (void *(*)(void *))dns_callback_thread,zeroconf_browse_phidget_ref);
        }


	   LOG(PHIDGET_LOG_DEBUG,"InitializeZeroconf Seems good...");
	}
	return EPHIDGET_OK;
}

int UninitializeZeroconf()
{
#ifdef ZEROCONF_RUNTIME_LINKING
#ifdef _WINDOWS
      //Free the library:
      BOOL freeResult = FreeLibrary(dllHandle); 
#elif _LINUX
#endif
#endif
	Dns_sdInitialized = FALSE;
	return EPHIDGET_OK;
}
