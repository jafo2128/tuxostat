#include "stdafx.h"
#include "zeroconf.h"
#include "phidgetinterface.h"
#include "PhidgetWebservice21.h"
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
#else
	#define DNSServiceRegisterPtr DNSServiceRegister
	#define DNSServiceProcessResultPtr DNSServiceProcessResult
	#define DNSServiceRefDeallocatePtr DNSServiceRefDeallocate
	#define DNSServiceAddRecordPtr DNSServiceAddRecord
	#define DNSServiceUpdateRecordPtr DNSServiceUpdateRecord
	#define DNSServiceRemoveRecordPtr DNSServiceRemoveRecord
#endif

/* 
 * TXT record version - this should be 1 for a long time
 *  - only need to change if we really change the TXT record format
 */
const char *dnssd_txt_ver = "1";

int Dns_sdInitialized = FALSE; 

#ifdef ZEROCONF_RUNTIME_LINKING
HMODULE dllHandle = NULL;
//DNS_SD functions
DNSServiceRegisterType DNSServiceRegisterPtr = NULL;
DNSServiceProcessResultType DNSServiceProcessResultPtr = NULL;
DNSServiceRefDeallocateType DNSServiceRefDeallocatePtr = NULL;
DNSServiceAddRecordType DNSServiceAddRecordPtr = NULL;
DNSServiceUpdateRecordType DNSServiceUpdateRecordPtr = NULL;
DNSServiceRemoveRecordType DNSServiceRemoveRecordPtr = NULL;
#endif

DNSServiceRef zeroconf_dict_ref  = NULL;

extern const char *serverName;

int InitializeZeroconf();
int UninitializeZeroconf();
void DNSSD_API dict_reg_reply(DNSServiceRef client, DNSServiceFlags flags, DNSServiceErrorType errorCode,
	const char *name, const char *regtype, const char *domain, void *context);
void DNSSD_API phid_reg_reply(DNSServiceRef client, DNSServiceFlags flags, DNSServiceErrorType errorCode,
	const char *name, const char *regtype, const char *domain, void *context);
	
int zeroconf_advertise_ws()
{
	InitializeZeroconf();
	if(Dns_sdInitialized)
	{
		pthread_t dns_thread;
		char txt[2048];

		snprintf(txt, sizeof(txt), "%ctxtvers=%s%cprotocolvers=%s%cauth=%s", 
			(unsigned short)(strlen("txtvers=") + strlen(dnssd_txt_ver)), dnssd_txt_ver,
			(unsigned short)(strlen("protocolvers=") + strlen(protocol_ver)), protocol_ver,
			(unsigned short)(strlen("auth=") + 1), (password==NULL?"n":"y"));

		DNSServiceRegisterPtr(&zeroconf_dict_ref, 0, 0, serverName, "_phidget_ws._tcp", "", "", htons(port), strlen(txt), txt, dict_reg_reply, NULL);
		DNSServiceProcessResultPtr(zeroconf_dict_ref);
		pthread_create(&dns_thread, NULL, (void *(*)(void *))dns_callback_thread,zeroconf_dict_ref);
	}
	return EPHIDGET_OK;
}

int zeroconf_unadvertise_phidget(CPhidgetHandle phid)
{
	if(Dns_sdInitialized)
	{
		DNSServiceRefDeallocatePtr(phid->dnsServiceRef);
	}
	return EPHIDGET_OK;
}

int zeroconf_advertise_phidget(CPhidgetHandle phid)
{
	if(Dns_sdInitialized)
	{
		//pthread_t dns_thread;
		char mdns_name[1024];
		DNSServiceErrorType err;
		const unsigned char txt[2048];
		int txt_len = sizeof(txt);
		const char *name;

		createDNSTXTRecord(phid, txt, &txt_len);
		CPhidget_getDeviceName(phid, &name);
		snprintf(mdns_name, sizeof(mdns_name), "%s (%d)", name, phid->serialNumber);

		err = DNSServiceRegisterPtr((DNSServiceRef *)&phid->dnsServiceRef, 0, 0, mdns_name, "_phidget._tcp", "", "", htons(port), txt_len, txt, phid_reg_reply, NULL);
		//TODO: if we're going to start a read thread - make sure to STOP it before killing thing in the detach handler
		//	We should probably call this once - but is it really needed to call it in a thread loop?
		//DNSServiceProcessResultPtr(phid->dnsServiceRef);
		//pthread_create(&dns_thread, NULL, (void *(*)(void *))dns_callback_thread,phid->dnsServiceRef);
	}
	return EPHIDGET_OK;
}

int dns_callback_thread(void * ref)
{
    while (DNSServiceProcessResultPtr(ref) == kDNSServiceErr_NoError)
        continue;
	return EPHIDGET_OK;
}

//TODO: implement this better!
int createDNSTXTRecord(CPhidgetHandle phid, const unsigned char *txt_buf, int *len)
{
	int i;
	const char *name;
	char *txt[9];

	char txt_ver[20], txt_name[128], txt_type[50], txt_serial[20], txt_server_id[1024], txt_label[50];
	char txt_txtver[20], txt_protocolver[25], txt_auth[10];

	unsigned char *ptr = (unsigned char *)txt_buf;

	txt[0] = txt_txtver; txt[4]=txt_name; txt[5]=txt_type; txt[6]=txt_serial; txt[3]=txt_server_id; txt[8]=txt_label;
	txt[7] = txt_ver; txt[1] = txt_protocolver; txt[2] = txt_auth;

	CPhidget_getDeviceName(phid, &name);
	snprintf(txt_ver, sizeof(txt_ver), "version=%d", phid->deviceVersion);
	snprintf(txt_name, sizeof(txt_name), "name=%s", name);
	snprintf(txt_type, sizeof(txt_type), "type=%s", phid->deviceType);
	snprintf(txt_serial, sizeof(txt_serial), "serial=%d", phid->serialNumber);
	snprintf(txt_server_id, sizeof(txt_server_id), "server_id=%s", serverName);
	snprintf(txt_label, sizeof(txt_label), "label=%s", phid->label);
	snprintf(txt_txtver, sizeof(txt_txtver), "txtvers=%s", dnssd_txt_ver);
	snprintf(txt_protocolver, sizeof(txt_protocolver), "protocolvers=%s", protocol_ver);
	snprintf(txt_auth, sizeof(txt_auth), "auth=%s", (password==NULL?"n":"y"));

	for (i = 0; i < 9; i++)
	{
		unsigned char *len = ptr++;
		*len = ( unsigned char ) strlen(txt[i]);
		strcpy((char*)ptr, txt[i]);
		ptr += *len;
	}

	*len = (ptr-txt_buf);

	return EPHIDGET_OK;
}

int updateDNSTXTRecords()
{
	CPhidgetHandle phid = NULL;
	CPhidgetHandle *phidArray;
	int count, i;
	DNSServiceErrorType err;

	if(!phidm) return EPHIDGET_OK;
	
	CPhidgetManager_getAttachedDevices(phidm, &phidArray, &count);

	for (i=0;i<count;i++)
	{
		phid = phidArray[i];
		if(Dns_sdInitialized)
		{	
			const unsigned char txt[2048];
			int txt_len = sizeof(txt);
			createDNSTXTRecord(phid, txt, &txt_len);
			err = DNSServiceUpdateRecordPtr((DNSServiceRef)phid->dnsServiceRef, NULL, 0, (uint16_t) txt_len, txt, 0);
		}
	}

	return EPHIDGET_OK;
}

void DNSSD_API dict_reg_reply(DNSServiceRef client, DNSServiceFlags flags, DNSServiceErrorType errorCode,
	const char *name, const char *regtype, const char *domain, void *context)
{
	//printf("Got a reply for %s.%s%s: ", name, regtype, domain);
	switch (errorCode)
		{
		case kDNSServiceErr_NoError:      
			//printf("Name now registered and active\n"); 
			break;
		case kDNSServiceErr_NameConflict: 
			//printf("Name in use, please choose another\n"); 
			exit(-1);
		default:                          
			//printf("Error %d\n", errorCode); 
			return;
		}
	strcpy((char *)serverName, name);

	updateDNSTXTRecords();
}

void DNSSD_API phid_reg_reply(DNSServiceRef client, DNSServiceFlags flags, DNSServiceErrorType errorCode,
	const char *name, const char *regtype, const char *domain, void *context)
{
	//printf("Got a reply for %s.%s%s: ", name, regtype, domain);
	switch (errorCode)
		{
		case kDNSServiceErr_NoError:      
			//printf("Name now registered and active\n"); 
			break;
		case kDNSServiceErr_NameConflict: 
			//printf("Name in use, please choose another\n"); 
			exit(-1);
		default:                          
			//printf("Error %d\n", errorCode); 
			return;
		}
}

int InitializeZeroconf()
{
#ifdef ZEROCONF_RUNTIME_LINKING

#ifdef _WINDOWS
	if(!(dllHandle = LoadLibrary(L"dnssd.dll")))
	{
		DWORD error = GetLastError();
		switch(error)
		{
		case ERROR_MOD_NOT_FOUND:
			DPRINT("LoadLibrary failed - module could not be found");
			break;
		default:
			DPRINT("LoadLibrary failed with error code: %d", error);
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

      Dns_sdInitialized = (
		  NULL != DNSServiceRegisterPtr && 
		  NULL != DNSServiceProcessResultPtr &&
		  NULL != DNSServiceRefDeallocatePtr &&
		  NULL != DNSServiceAddRecordPtr &&
		  NULL != DNSServiceUpdateRecordPtr &&
		  NULL != DNSServiceRemoveRecordPtr);
   }
#endif
   if(!Dns_sdInitialized)
   {
		DPRINT("InitializeZeroconf failed somehow...");
		return EPHIDGET_UNEXPECTED;
   }
		

   DPRINT("InitializeZeroconf Seems good...");
#else
	Dns_sdInitialized = TRUE;
#endif

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