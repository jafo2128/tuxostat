#include "stdafx.h"
#include "csocket.h"
#include "csocketevents.h"
#include "cphidgetlist.h"
#include "cphidgetmanager.h"
#include "cphidgetdictionary.h"
#ifdef USE_ZEROCONF
#include "zeroconf.h"
#endif

void DNSServiceResolve_CallBack(DNSServiceRef sdRef,
				DNSServiceFlags                     flags,
				uint32_t                            interfaceIndex,
				DNSServiceErrorType                 errorCode,
				const char                          *fullname,
				const char                          *hosttarget,
				uint16_t                            port,
				uint16_t                            txtLen,
				const char                          *txtRecord,
				void                                *context);

typedef struct _CServerInfo
{
	CPhidgetSocketClientHandle server;
	CPhidgetListHandle phidgets;
	CPhidgetManagerListHandle managers;
	CPhidgetDictionaryListHandle dictionaries;
} CServerInfo, *CServerInfoHandle;

typedef struct _CServerList
{
	struct _CServerList *next;
	CServerInfoHandle serverInfo;
} CServerList, *CServerListHandle;

CThread_func_return_t CentralRemoteThreadFunction(CThread_func_arg_t arg);
static CThread CentralRemoteThread;

CServerListHandle servers = NULL;
/* Protects servers */
int serverLockInitialized = PFALSE;
CThread_mutex_t serverLock;

CPhidgetRemoteListHandle zeroconfServers = NULL;
CPhidgetListHandle zeroconfPhidgets = NULL;
/* Protects zeroconf lists */
int zeroconfListLockInitialized = PFALSE;
CThread_mutex_t zeroconfServersLock;
CThread_mutex_t zeroconfPhidgetsLock;

//Lists of objects that we have called openRemote or openRemoteIP on
CPhidgetListHandle activeRemotePhidgets = NULL;
CPhidgetManagerListHandle activeRemoteManagers = NULL;
CPhidgetDictionaryListHandle activeRemoteDictionaries = NULL;
/* Protects the lists */
int activeRemoteLocksInitialized = PFALSE;
CThread_mutex_t activeRemotePhidgetsLock;
CThread_mutex_t activeRemoteManagersLock;
CThread_mutex_t activeRemoteDictionariesLock;

int NetworkInitialized = PFALSE;

#ifdef _WINDOWS
int start_WSA_server()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		LOG(PHIDGET_LOG_DEBUG,"Cannot start WSA");
		return EPHIDGET_NETWORK;
	}
	return EPHIDGET_OK;
}
#endif

int CServerInfo_areEqual(void *arg1, void *arg2)
{
	CServerInfoHandle server1 = (CServerInfoHandle)arg1;
	CServerInfoHandle server2 = (CServerInfoHandle)arg2;
	
	TESTPTRS(server1, server2)
	TESTPTRS(server1->server, server2->server)
	
	return CPhidgetSocketClient_areEqual(server1->server, server2->server);
}

void CServerInfo_free(void *arg)
{
	CServerInfoHandle server = (CServerInfoHandle)arg;
	
	if(!server)
		return;
	if(server->server)
		CPhidgetSocketClient_free(server->server); server->server = NULL;
	//CList_emptyList((CListHandle *)&server->listen_ids, PFALSE, NULL);
	free(server); server = NULL;
	return;
}

int CCONV CPhidgetRemote_create(CPhidgetRemoteHandle *arg)
{
	CPhidgetRemoteHandle remote;
	
	if(!(remote = malloc(sizeof(CPhidgetRemote))))
		return EPHIDGET_NOMEMORY;
	ZEROMEM(remote, sizeof(CPhidgetRemote));

	*arg = remote;
	 
	 return EPHIDGET_OK;
}

void CCONV CPhidgetRemote_free(void *arg)
{

	CPhidgetRemoteHandle remote = (CPhidgetRemoteHandle)arg;

	if(!remote) return;
	if(remote->requested_port) free(remote->requested_port);
	if(remote->requested_address) free(remote->requested_address);
	if(remote->requested_serverID) free(remote->requested_serverID);
	if(remote->password) free(remote->password);
	CPhidgetSocketClient_free(remote->server);

	free(remote);

	return;
}

int CCONV CPhidgetRemote_areEqual(void *arg1, void *arg2)
{
	CPhidgetRemoteHandle remote1 = (CPhidgetRemoteHandle)arg1;
	CPhidgetRemoteHandle remote2 = (CPhidgetRemoteHandle)arg2;
	
	TESTPTRS(remote1, remote2)

	if(remote1->zeroconf_name != NULL || remote2->zeroconf_name != NULL)
		if((strcmp(remote1->zeroconf_name,remote2->zeroconf_name)))
			return 0;
		
	if(remote1->zeroconf_domain != NULL || remote2->zeroconf_domain != NULL)
		if((strcmp(remote1->zeroconf_domain,remote2->zeroconf_domain)))
			return 0;
		
	if(remote1->zeroconf_type != NULL || remote2->zeroconf_type != NULL)
		if((strcmp(remote1->zeroconf_type,remote2->zeroconf_type)))
			return 0;
		
	if(remote1->requested_port != NULL || remote2->requested_port != NULL)
		if((strcmp(remote1->requested_port,remote2->requested_port)))
			return 0;
		
	if(remote1->requested_address != NULL || remote2->requested_address != NULL)
		if((strcmp(remote1->requested_address,remote2->requested_address)))
			return 0;
		
	if(remote1->requested_serverID != NULL || remote2->requested_serverID != NULL)
		if((strcmp(remote1->requested_serverID,remote2->requested_serverID)))
			return 0;

	return 1;
}

int CCONV CPhidgetSocketClient_create(CPhidgetSocketClientHandle *arg)
{
	CPhidgetSocketClientHandle socket_client;
	
	if(!(socket_client = malloc(sizeof(CPhidgetSocketClient))))
		return EPHIDGET_NOMEMORY;
	ZEROMEM(socket_client, sizeof(CPhidgetSocketClient));

	CThread_mutex_init(&socket_client->lock);
	CThread_mutex_init(&socket_client->pdc_lock);

	CPhidgetSocketClient_clearStatusFlag(socket_client, PHIDGETSOCKET_ATTACHED_FLAG);

	*arg = socket_client;
	 
	 return EPHIDGET_OK;
}

void CCONV CPhidgetSocketClient_free(void *arg)
{
	CPhidgetSocketClientHandle socket_client = (CPhidgetSocketClientHandle)arg;

	if(!socket_client) return;
	if(socket_client->port) free(socket_client->port);
	if(socket_client->address) free(socket_client->address);
	//if(socket_client->serverID) free(socket_client->serverID);
	if(socket_client->pdcs) free(socket_client->pdcs);

	CThread_mutex_destroy(&socket_client->lock);
	CThread_mutex_destroy(&socket_client->pdc_lock);

	free(socket_client);
	 
	return;
}

int CCONV CPhidgetSocketClient_areEqual(void *arg1, void *arg2)
{
	CPhidgetSocketClientHandle socket1 = (CPhidgetSocketClientHandle)arg1;
	CPhidgetSocketClientHandle socket2 = (CPhidgetSocketClientHandle)arg2;
	
	TESTPTRS(socket1, socket2)
	TESTPTRS(socket1->address, socket2->address)
	TESTPTRS(socket1->port, socket2->port)
	
	if(!(strcmp(socket1->address,socket2->address))
		&& !(strcmp(socket1->port, socket2->port)))
		return 1;
		
	return 0;
}

int CPhidgetSocketClient_statusFlagIsSet(CPhidgetSocketClientHandle socket, int flag)
{
	int flag_status = PFALSE;
	assert(socket);

	if(socket->status & flag) flag_status = PTRUE;
	return flag_status;
}

int CPhidgetSocketClient_setStatusFlag(CPhidgetSocketClientHandle socket, int flag)
{
	TESTPTR(socket)

	CThread_mutex_lock(&socket->lock);
	socket->status |= flag;
	CThread_mutex_unlock(&socket->lock);
	return EPHIDGET_OK;
}

int CPhidgetSocketClient_clearStatusFlag(CPhidgetSocketClientHandle socket, int flag)
{
	TESTPTR(socket)

	CThread_mutex_lock(&socket->lock);
	socket->status &= (~flag);
	CThread_mutex_unlock(&socket->lock);
	return EPHIDGET_OK;
}

void internal_async_network_error_handler(const char *error, void *ptr)
{
	CPhidgetHandle phid = (CPhidgetHandle)ptr;
	if(phid && phid->fptrError)
	{
		phid->fptrError(phid, phid->fptrErrorptr, EPHIDGET_NETWORK, error);
		return;
	}
	LOG(PHIDGET_LOG_WARNING,"Got an async network error: %s\n\tTip: Set up an error handler to catch this properly.", error);
	//abort();
}

static int initialize_locks()
{
	if(!serverLockInitialized)
	{
		CThread_mutex_init(&serverLock);
		serverLockInitialized = PTRUE;
	}
	if(!zeroconfListLockInitialized)
	{
		CThread_mutex_init(&zeroconfServersLock);
		CThread_mutex_init(&zeroconfPhidgetsLock);
		zeroconfListLockInitialized = PTRUE;
	}
	if(!activeRemoteLocksInitialized)
	{
		CThread_mutex_init(&activeRemotePhidgetsLock);
		CThread_mutex_init(&activeRemoteManagersLock);
		CThread_mutex_init(&activeRemoteDictionariesLock);
		activeRemoteLocksInitialized = PTRUE;
	}
	return EPHIDGET_OK;
}

int InitializeNetworking()
{
	int res;
	const char *setpattern = "^/PSK/([a-zA-Z_0-9]*)/([0-9]*)/([a-zA-Z_0-9]*)/?([a-zA-Z_0-9]*)/?([a-zA-Z_0-9]*)$";
	const char *managerpattern = "^/PSK/List/([a-zA-Z_0-9]*)/([0-9]*)/([0-9]*)/([0-9]*)$";

#ifdef _WINDOWS
	if (start_WSA_server())
	{
		LOG(PHIDGET_LOG_WARNING,"Cannot start Windows Sockets");
		return EPHIDGET_NETWORK;
	}
#endif
	
	if (!pdc_init()) {
		return EPHIDGET_UNEXPECTED;
	}

	if ((res = regcomp(&phidgetsetex, setpattern, REG_EXTENDED)) != 0) {
		LOG(PHIDGET_LOG_DEBUG,"set command pattern compilation error %d",
			res);
		abort();
	}
	if ((res = regcomp(&managerex, managerpattern, REG_EXTENDED)) != 0) {
		LOG(PHIDGET_LOG_DEBUG,"set command pattern compilation error %d",
			res);
		abort();
	}

	NetworkInitialized = PTRUE;

	return EPHIDGET_OK;
}

void cleanup_after_socket(void *ptr)
{
	CPhidgetSocketClientHandle serverInfo = ptr;
	CServerList *travServers;
	CServerInfoHandle foundServer = NULL;
	CPhidgetListHandle travPhidgets;
	CPhidgetDictionaryListHandle travDicts;
	CPhidgetManagerListHandle travManagers;

	CPhidgetListHandle detachEvents = NULL;
	CPhidgetListHandle disconnectEvents = NULL;

	/* For each Phidget associated with this server, send a detach event, and a disconnect event */
	// We then get rid of the socket object everywhere
	// If this is called because we already closed all the connections to this socket, no events will be raised because
	//  all the handles are already nulled...
	CThread_mutex_lock(&serverLock);
	for(travServers = servers; travServers; travServers = travServers->next)
	{
		if(travServers->serverInfo->server->socket == serverInfo->socket)
		{
			foundServer = travServers->serverInfo;
			CPhidgetSocketClient_clearStatusFlag(travServers->serverInfo->server, PHIDGETSOCKET_ATTACHED_FLAG);
			for(travPhidgets = travServers->serverInfo->phidgets; travPhidgets; travPhidgets = travPhidgets->next)
			{
				if(CPhidget_statusFlagIsSet(travPhidgets->phid, PHIDGET_ATTACHED_FLAG))
				{
					CPhidget_clearStatusFlag(travPhidgets->phid, PHIDGET_ATTACHED_FLAG);
					CPhidget_setStatusFlag(travPhidgets->phid, PHIDGET_DETACHING_FLAG);
					if(travPhidgets->phid->fptrDetach)
						CList_addToList((CListHandle *)&detachEvents, travPhidgets->phid, CPhidget_areEqual);
				}
				if(travPhidgets->phid->fptrDisconnect)
					CList_addToList((CListHandle *)&disconnectEvents, travPhidgets->phid, CPhidgetHandle_areEqual);	
			}
			for(travDicts = travServers->serverInfo->dictionaries; travDicts; travDicts = travDicts->next)
			{
				CPhidget_clearStatusFlag((CPhidgetHandle)travDicts->dict, PHIDGET_ATTACHED_FLAG);
				if(travDicts->dict->fptrDisconnect)
					CList_addToList((CListHandle *)&disconnectEvents, travDicts->dict, CPhidgetHandle_areEqual);
			}
			for(travManagers = travServers->serverInfo->managers; travManagers; travManagers = travManagers->next)
			{
				CPhidget_clearStatusFlag((CPhidgetHandle)travManagers->phidm, PHIDGET_ATTACHED_FLAG);
				if(travManagers->phidm->fptrDisconnect)
					CList_addToList((CListHandle *)&disconnectEvents, travManagers->phidm, CPhidgetHandle_areEqual);
			}
			break;
		}
	}

	for(travPhidgets = disconnectEvents; travPhidgets; travPhidgets = travPhidgets->next)
	{
		travPhidgets->phid->fptrDisconnect((CPhidgetHandle)travPhidgets->phid, travPhidgets->phid->fptrDisconnectptr);
		//internal_async_network_error_handler("The Network Connection has been Closed", travPhidgets->phid);
	}
	for(travPhidgets = detachEvents; travPhidgets; travPhidgets = travPhidgets->next)
	{
		travPhidgets->phid->fptrDetach((CPhidgetHandle)travPhidgets->phid, travPhidgets->phid->fptrDetachptr);
		CPhidget_clearStatusFlag(travPhidgets->phid, PHIDGET_DETACHING_FLAG);
		travPhidgets->phid->networkInfo->server = NULL;
	}

	CList_emptyList((CListHandle *)&detachEvents, FALSE, NULL);
	CList_emptyList((CListHandle *)&disconnectEvents, FALSE, NULL);

	if(foundServer)
		CList_removeFromList((CListHandle *)&servers, foundServer, CServerInfo_areEqual, PTRUE, CServerInfo_free);
	CThread_mutex_unlock(&serverLock);
}

int connectToServer(CPhidgetRemoteHandle remoteInfo, CServerInfoHandle *foundServer)
{
	char errdesc[1024];
	int result = EPHIDGET_OK;
	
	CServerInfoHandle newServerInfo;
	
	//Initialize the network if not already done
	if(!NetworkInitialized)
		if((result = InitializeNetworking()))
			return result;
	
	if(!(newServerInfo = malloc(sizeof(CServerInfo))))
		return EPHIDGET_NOMEMORY;
	ZEROMEM(newServerInfo, sizeof(CServerInfo));
	if((result = CPhidgetSocketClient_create(&newServerInfo->server)))
		return result;
	
	if(remoteInfo->requested_address != NULL)
	{
		if(!(newServerInfo->server->address = strdup(remoteInfo->requested_address)))
			return EPHIDGET_NOMEMORY;
		if(!(newServerInfo->server->port = strdup(remoteInfo->requested_port)))
			return EPHIDGET_NOMEMORY;
	}
#ifdef USE_ZEROCONF
	else //seems we've found an mDNS server we want to connect to
	{
		getZeroconfHostPort(remoteInfo);
		if(!(newServerInfo->server->address = strdup(remoteInfo->zeroconf_host)))
			return EPHIDGET_NOMEMORY;
		if(!(newServerInfo->server->port = strdup(remoteInfo->zeroconf_port)))
			return EPHIDGET_NOMEMORY;
	}
#else
	else
	{
		return EPHIDGET_INVALIDARG;
	}
#endif

	//check to see if there is already a connection to this server
	result = CList_findInList((CListHandle)servers, newServerInfo, CServerInfo_areEqual, (void **)foundServer);
	switch(result)
	{
		case EPHIDGET_OK: //Found
			remoteInfo->server = (*foundServer)->server;
			CServerInfo_free(newServerInfo); newServerInfo = NULL;
			break;
		case EPHIDGET_NOTFOUND: //Not Found
			remoteInfo->server = newServerInfo->server;
			if (!stream_server_connect(remoteInfo->server->address, remoteInfo->server->port, &remoteInfo->server->socket, errdesc,
				sizeof (errdesc))) {
				LOG(PHIDGET_LOG_DEBUG,"connect(%s:%s): %s", remoteInfo->server->address, remoteInfo->server->port,
					errdesc);
				CPhidgetSocketClient_setStatusFlag(remoteInfo->server, PHIDGETSOCKET_CONNECTIONERROR_FLAG);
				CList_removeFromList((CListHandle *)&servers, newServerInfo, CServerInfo_areEqual, PTRUE, CServerInfo_free);
				remoteInfo->server = NULL;
				return EPHIDGET_NETWORK;
			}
			
			if (!(remoteInfo->server->pdcs = pdc_session_alloc(remoteInfo->server->socket, pu_read, remoteInfo->server->socket, pu_write, pu_close, remoteInfo->server, cleanup_after_socket, remoteInfo->password))) {
				fflush(stderr);
				CPhidgetSocketClient_setStatusFlag(remoteInfo->server, PHIDGETSOCKET_CONNECTIONERROR_FLAG);
				CList_removeFromList((CListHandle *)&servers, newServerInfo, CServerInfo_areEqual, PTRUE, CServerInfo_free);
				remoteInfo->server = NULL;
				return EPHIDGET_NOTFOUND;
			}
			
			while(!pdc_get_status(remoteInfo->server->pdcs))
			{
				SLEEP(10);
			}

			if(pdc_get_status(remoteInfo->server->pdcs) == 2)
			{
				LOG(PHIDGET_LOG_DEBUG,"bad password");
				CList_removeFromList((CListHandle *)&servers, newServerInfo, CServerInfo_areEqual, PTRUE, CServerInfo_free);
				remoteInfo->server = NULL;
				return EPHIDGET_BADPASSWORD;
			}
	
			/* maybe give to user a way to change this? */
			if(!pdc_enable_periodic_reports(remoteInfo->server->pdcs, 10, errdesc, sizeof (errdesc)))
			{
				LOG(PHIDGET_LOG_DEBUG,"pdc_enable_periodic_reports: %s", errdesc);
				CList_removeFromList((CListHandle *)&servers, newServerInfo, CServerInfo_areEqual, PTRUE, CServerInfo_free);
				remoteInfo->server = NULL;
				return EPHIDGET_UNEXPECTED;
			}
			
			if((result = CList_addToList((CListHandle *)&servers, newServerInfo, CServerInfo_areEqual)))
				return result;
			if((result = CList_findInList((CListHandle)servers, newServerInfo, CServerInfo_areEqual, (void **)foundServer)))
				return result;

			CPhidgetSocketClient_setStatusFlag(remoteInfo->server, PHIDGETSOCKET_ATTACHED_FLAG);
			break;
		default:
			return result;
	}
	return EPHIDGET_OK;
}

int setupKeysAndListeners_phidget(CPhidgetHandle phid, pdc_listen_id_t *id)
{
	char errdesc[1024];
	char listenKey[1024];
	struct sockaddr name;
	struct sockaddr_in *name_in;
	socklen_t namelen = sizeof(name);

	char key[1024], val[1024];

	TESTPTR(phid)
	TESTPTR(phid->networkInfo)
	TESTPTR(phid->networkInfo->server)

	//listen for everything to do with this specific phidget
	if(phid->specificDevice)
	{
		snprintf(listenKey, sizeof(listenKey), "^/PSK/%s/%d/", Phid_DeviceName[phid->deviceID], phid->serialNumber);
	}
	else
	{
		snprintf(listenKey, sizeof(listenKey), "^/PSK/%s/", Phid_DeviceName[phid->deviceID]);
	}
	
	CThread_mutex_lock(&phid->networkInfo->server->pdc_lock);
	if (!(*id = pdc_listen(phid->networkInfo->server->pdcs, listenKey, network_phidget_event_handler, phid, errdesc, sizeof (errdesc))))
	{
		LOG(PHIDGET_LOG_DEBUG,"pdc_listen: %s", errdesc);
		CThread_mutex_unlock(&phid->networkInfo->server->pdc_lock);
		return EPHIDGET_UNEXPECTED;
	}
	CThread_mutex_unlock(&phid->networkInfo->server->pdc_lock);
	//Open the remote device
	//get socket info
	getsockname(phid->networkInfo->server->socket, &name, &namelen);
	name_in = (struct sockaddr_in *)&name;
	if(phid->specificDevice)
	{
		snprintf(key, sizeof(key), "/PCK/Client/%s/%d/%s/%d", inet_ntoa(name_in->sin_addr), (int)name_in->sin_port, 
			Phid_DeviceName[phid->deviceID], phid->serialNumber);
	}
	else
	{
		snprintf(key, sizeof(key), "/PCK/Client/%s/%d/%s", inet_ntoa(name_in->sin_addr), (int)name_in->sin_port, 
			Phid_DeviceName[phid->deviceID]);
	}
	snprintf(val, sizeof(val), "Open");
	pdc_async_set(phid->networkInfo->server->pdcs, key, val, (int)strlen(val), PTRUE, internal_async_network_error_handler, phid);

	return EPHIDGET_OK;
}

int setupKeysAndListeners_manager(CPhidgetManagerHandle phidm, pdc_listen_id_t *id)
{
	char errdesc[1024];
	char listenKey[1024];

	TESTPTR(phidm)
	TESTPTR(phidm->networkInfo)
	TESTPTR(phidm->networkInfo->server)

	//listen for everything to do with the PhidgetManager
	snprintf(listenKey, sizeof(listenKey), "^/PSK/List/");

	CThread_mutex_lock(&phidm->networkInfo->server->pdc_lock);
	if (!(*id = pdc_listen(phidm->networkInfo->server->pdcs, listenKey, network_manager_event_handler, phidm, errdesc, sizeof (errdesc))))
	{
		LOG(PHIDGET_LOG_DEBUG,"pdc_listen: %s", errdesc);
		CThread_mutex_unlock(&phidm->networkInfo->server->pdc_lock);
		return EPHIDGET_UNEXPECTED;
	}
	CThread_mutex_unlock(&phidm->networkInfo->server->pdc_lock);

	return EPHIDGET_OK;
}






int attachActiveRemotePhidget(CPhidgetHandle phid)
{
	int result = EPHIDGET_OK, result2;
	CServerInfoHandle foundServer;

	result = connectToServer(phid->networkInfo, &foundServer);
	switch(result)
	{
		case EPHIDGET_OK:
			if((result2 = setupKeysAndListeners_phidget(phid, &phid->networkInfo->listen_id)))
				return result2;	
			if((result = CList_addToList((CListHandle *)&foundServer->phidgets, phid, CPhidget_areEqual)))
				return result;
			if(phid->fptrConnect)
				phid->fptrConnect(phid, phid->fptrConnectptr);
			break;
		case EPHIDGET_BADPASSWORD:
			throw_error_event(phid, Phid_ErrorDescriptions[EPHIDGET_BADPASSWORD], EPHIDGET_BADPASSWORD);
			return result;
		default:
			return result;
	}

	return EPHIDGET_OK;
}
int attachActiveRemoteManager(CPhidgetManagerHandle phidm)
{
	int result = EPHIDGET_OK, result2;
	CServerInfoHandle foundServer;
	
	result = connectToServer(phidm->networkInfo, &foundServer);
	switch(result)
	{
		case EPHIDGET_OK: /* Only need to register this once */
			if((result2 = setupKeysAndListeners_manager(phidm, &phidm->networkInfo->listen_id)))
				return result2;
			if((result = CList_addToList((CListHandle *)&foundServer->managers, phidm, CPhidgetManager_areEqual)))
				return result;
			if(phidm->fptrConnect)
				phidm->fptrConnect(phidm, phidm->fptrConnectptr);
			break;
		case EPHIDGET_BADPASSWORD:
			throw_error_event((CPhidgetHandle)phidm, Phid_ErrorDescriptions[EPHIDGET_BADPASSWORD], EPHIDGET_BADPASSWORD);
			return result;
		default:
			return result;
	}

	return EPHIDGET_OK;
}
int attachActiveRemoteDictionary(CPhidgetDictionaryHandle dict)
{
	int result = EPHIDGET_OK;
	CServerInfoHandle foundServer;
	
	result = connectToServer(dict->networkInfo, &foundServer);
	switch(result)
	{
		case EPHIDGET_OK:
			if((result = CList_addToList((CListHandle *)&foundServer->dictionaries, dict, CPhidgetDictionary_areEqual)))
				return result;
			if(dict->fptrConnect)
				dict->fptrConnect(dict, dict->fptrConnectptr);
			break;
		case EPHIDGET_BADPASSWORD:
			throw_error_event((CPhidgetHandle)dict, Phid_ErrorDescriptions[EPHIDGET_BADPASSWORD], EPHIDGET_BADPASSWORD);
			return result;
		default:
			return result;
	}

	return EPHIDGET_OK;
}

// This will try to connect unconnected openRemoteIP devices and find any zeroconf matches
// TODO: beware: each call to connect will block for a while if it can't connect - maybe we need more threads! :-D
int
FindActiveRemoteDevices()
{
	CPhidgetList *trav = 0, *trav2;
	CPhidgetManagerList *trav3;
	CPhidgetDictionaryList *trav4, *trav5;
	CPhidgetRemoteList *trav6;
	int result = 0;


	//Zeroconf Phidgets
	CThread_mutex_lock(&activeRemotePhidgetsLock);
	CThread_mutex_lock(&zeroconfPhidgetsLock);
	for (trav2=zeroconfPhidgets; trav2; trav2 = trav2->next)
	{
		for (trav=activeRemotePhidgets; trav; trav = trav->next)
		{
			if((!trav->phid->networkInfo->server || !CPhidgetSocketClient_statusFlagIsSet(trav->phid->networkInfo->server, PHIDGETSOCKET_ATTACHED_FLAG)) 
				&& CPhidget_areExtraEqual(trav->phid, trav2->phid) 
				&& trav->phid->networkInfo->requested_address==NULL
				&& (trav->phid->networkInfo->requested_serverID == NULL || !strcmp(trav->phid->networkInfo->requested_serverID,trav2->phid->networkInfo->zeroconf_server_id)))
			{
				CThread_mutex_lock(&serverLock);
				trav->phid->networkInfo->zeroconf_name = trav2->phid->networkInfo->zeroconf_name;
				trav->phid->networkInfo->zeroconf_type = trav2->phid->networkInfo->zeroconf_type;
				trav->phid->networkInfo->zeroconf_domain = trav2->phid->networkInfo->zeroconf_domain;
				result = attachActiveRemotePhidget(trav->phid);
				CThread_mutex_unlock(&serverLock);
				goto next_zeroconf_phidget;
			}
		}
		for (trav=activeRemotePhidgets; trav; trav = trav->next)
		{
			if((!trav->phid->networkInfo->server || !CPhidgetSocketClient_statusFlagIsSet(trav->phid->networkInfo->server, PHIDGETSOCKET_ATTACHED_FLAG)) 
				&& CPhidget_areEqual(trav->phid, trav2->phid) 
				&& trav->phid->networkInfo->requested_address==NULL
				&& (trav->phid->networkInfo->requested_serverID == NULL || !strcmp(trav->phid->networkInfo->requested_serverID,trav2->phid->networkInfo->zeroconf_server_id)))
			{
				CThread_mutex_lock(&serverLock);
				trav->phid->networkInfo->zeroconf_name = trav2->phid->networkInfo->zeroconf_name;
				trav->phid->networkInfo->zeroconf_type = trav2->phid->networkInfo->zeroconf_type;
				trav->phid->networkInfo->zeroconf_domain = trav2->phid->networkInfo->zeroconf_domain;
				result = attachActiveRemotePhidget(trav->phid);
				CThread_mutex_unlock(&serverLock);
				goto next_zeroconf_phidget;
			}
		}
next_zeroconf_phidget:;
	}
	CThread_mutex_unlock(&zeroconfPhidgetsLock);
	CThread_mutex_unlock(&activeRemotePhidgetsLock);

	
	//Zeroconf Dictionaries
	CThread_mutex_lock(&activeRemoteDictionariesLock);
	CThread_mutex_lock(&zeroconfServersLock);
	for (trav6=zeroconfServers; trav6; trav6 = trav6->next)
	{
		for (trav5=activeRemoteDictionaries; trav5; trav5 = trav5->next)
		{
			if((!trav5->dict->networkInfo->server || !CPhidgetSocketClient_statusFlagIsSet(trav5->dict->networkInfo->server, PHIDGETSOCKET_ATTACHED_FLAG)) 
				&& trav5->dict->networkInfo->requested_address==NULL
				&& (trav5->dict->networkInfo->requested_serverID == NULL || !strcmp(trav5->dict->networkInfo->requested_serverID,trav6->networkInfo->zeroconf_name)))
			{
				CThread_mutex_lock(&serverLock);
				trav5->dict->networkInfo->zeroconf_name = trav6->networkInfo->zeroconf_name;
				trav5->dict->networkInfo->zeroconf_type = trav6->networkInfo->zeroconf_type;
				trav5->dict->networkInfo->zeroconf_domain = trav6->networkInfo->zeroconf_domain;
				result = attachActiveRemoteDictionary(trav5->dict);
				CThread_mutex_unlock(&serverLock);
			}
		}
	}
	CThread_mutex_unlock(&zeroconfServersLock);
	CThread_mutex_unlock(&activeRemoteDictionariesLock);

	//then others
	//Phidgets
	CThread_mutex_lock(&activeRemotePhidgetsLock);
	for (trav=activeRemotePhidgets; trav; trav = trav->next)
	{
		if(trav->phid->networkInfo->requested_address!=NULL)
		{
			if(!trav->phid->networkInfo->server || !CPhidgetSocketClient_statusFlagIsSet(trav->phid->networkInfo->server, PHIDGETSOCKET_ATTACHED_FLAG))
			{
				CThread_mutex_lock(&serverLock);
				result = attachActiveRemotePhidget(trav->phid);
				CThread_mutex_unlock(&serverLock);
			}
		}
	}
	CThread_mutex_unlock(&activeRemotePhidgetsLock);
	//Managers
	CThread_mutex_lock(&activeRemoteManagersLock);
	for (trav3=activeRemoteManagers; trav3; trav3 = trav3->next)
	{
		if(trav3->phidm->networkInfo->requested_address!=NULL)
		{
			if(!trav3->phidm->networkInfo->server || !CPhidgetSocketClient_statusFlagIsSet(trav3->phidm->networkInfo->server, PHIDGETSOCKET_ATTACHED_FLAG))
			{
				CThread_mutex_lock(&serverLock);
				result = attachActiveRemoteManager(trav3->phidm);
				CThread_mutex_unlock(&serverLock);
			}
		}
	}
	CThread_mutex_unlock(&activeRemoteManagersLock);
	//Dictionaries
	CThread_mutex_lock(&activeRemoteDictionariesLock);
	for (trav4=activeRemoteDictionaries; trav4; trav4 = trav4->next)
	{
		if(trav4->dict->networkInfo->requested_address!=NULL)
		{
			if(!trav4->dict->networkInfo->server || !CPhidgetSocketClient_statusFlagIsSet(trav4->dict->networkInfo->server, PHIDGETSOCKET_ATTACHED_FLAG))
			{
				CThread_mutex_lock(&serverLock);
				result = attachActiveRemoteDictionary(trav4->dict);
				CThread_mutex_unlock(&serverLock);
			}
		}
	}
	CThread_mutex_unlock(&activeRemoteDictionariesLock);
	return result;
}

int
JoinCentralRemoteThread()
{
	if(CentralRemoteThread.m_ThreadHandle)
	{
		CThread_join(&CentralRemoteThread);
		CentralRemoteThread.m_ThreadHandle = 0;
	}
	return EPHIDGET_OK;
}

int
StartCentralRemoteThread()
{

#ifdef _WINDOWS
	if (CentralRemoteThread.m_ThreadHandle) {
		int threadStatus = 0;
		int result = 0;
		result = GetExitCodeThread(CentralRemoteThread.m_ThreadHandle,
		    (LPDWORD)&threadStatus);
		if (result) {
			if (threadStatus != STILL_ACTIVE) {
				CloseHandle(CentralRemoteThread.m_ThreadHandle);
				CentralRemoteThread.m_ThreadHandle = 0;
			}
		}
	}
#endif

	if (!CentralRemoteThread.m_ThreadHandle || 
		CentralRemoteThread.thread_status == FALSE)
	{
		if (CThread_create(&CentralRemoteThread, CentralRemoteThreadFunction, 0))
			return EPHIDGET_UNEXPECTED;
		CentralRemoteThread.thread_status = TRUE;
	}
	return EPHIDGET_OK;
}

CThread_func_return_t CentralRemoteThreadFunction(CThread_func_arg_t lpdwParam)
{
	initialize_locks();
	while(activeRemotePhidgets || activeRemoteManagers || activeRemoteDictionaries) {
		FindActiveRemoteDevices(); //this looks for attached active devices and opens them
		SLEEP(250);
	}
	CentralRemoteThread.thread_status = FALSE;
	return EPHIDGET_OK;
}

int RegisterRemotePhidget(CPhidgetHandle phid)
{
	int result = EPHIDGET_OK;

	//clear all variables
	phid->fptrClear((CPhidgetHandle)phid);

	CThread_mutex_lock(&activeRemotePhidgetsLock);
	
	result = CList_addToList((CListHandle *)&activeRemotePhidgets, phid, CPhidgetHandle_areEqual);

	if (result)
	{
		CThread_mutex_unlock(&activeRemotePhidgetsLock);
		return result;
	}
	CThread_mutex_unlock(&activeRemotePhidgetsLock);

	result = StartCentralRemoteThread();
	return result;
}

int RegisterRemoteManager(CPhidgetManagerHandle phidm)
{
	int result = EPHIDGET_OK;

	CThread_mutex_lock(&activeRemoteManagersLock);
	
	result = CList_addToList((CListHandle *)&activeRemoteManagers, phidm, CPhidgetHandle_areEqual);

	if (result)
	{
		CThread_mutex_unlock(&activeRemoteManagersLock);
		return result;
	}
	CThread_mutex_unlock(&activeRemoteManagersLock);

	result = StartCentralRemoteThread();
	return result;
}

int RegisterRemoteDictionary(CPhidgetDictionaryHandle dict)
{
	int result = EPHIDGET_OK;

	CThread_mutex_lock(&activeRemoteDictionariesLock);
	
	result = CList_addToList((CListHandle *)&activeRemoteDictionaries, dict, CPhidgetHandle_areEqual);

	if (result)
	{
		CThread_mutex_unlock(&activeRemoteDictionariesLock);
		return result;
	}
	CThread_mutex_unlock(&activeRemoteDictionariesLock);

	result = StartCentralRemoteThread();
	return result;
}

// This will close a server connection (and cleanup the socket, etc) if there are no more
// Phidgets, Managers, or Dictionaries in it's lists
int closeServer(CServerInfoHandle server)
{
	int result = EPHIDGET_OK;
	char errdesc[1024];
	void *pdcs = server->server->pdcs;
	//no more references to this server
	if(!server->phidgets && !server->dictionaries && !server->managers)
	{
		CThread_mutex_lock(&server->server->pdc_lock);
		if(!(pdc_quit(server->server->pdcs,errdesc,sizeof(errdesc))))
		{
			LOG(PHIDGET_LOG_DEBUG,"pdc_quit: %s", errdesc);
		}
		CThread_mutex_unlock(&server->server->pdc_lock);
		//We need to wait for the read thread to return
		//unlock serverLock so it can be claimed in the cleanup function
		//don't want the cleanup function to free pdcs while we're waiting on it..
		server->server->pdcs = NULL;
		CThread_mutex_unlock(&serverLock);
		pdc_readthread_join(pdcs, NULL);
		CThread_mutex_lock(&serverLock);
		free(pdcs);
		//this is done in the cleanup callback
		//if((result = CList_removeFromList((CListHandle *)&servers, server, CServerInfo_areEqual, PTRUE, CServerInfo_free)))
		//	return result;
	}
	return result;
}

int disconnectRemoteObject(void *object, size_t objectListOffset, int(*compareObjects)(void *, void *))
{
	int result = EPHIDGET_OK;
	CServerInfoHandle foundServer;
	char errdesc[1024];
	CServerInfo newServerInfo;

	CPhidgetHandle phid = object;

	if(phid->networkInfo->server)
	{
		newServerInfo.server = phid->networkInfo->server;

		//check to see if there is already a connection to this server
		result = CList_findInList((CListHandle)servers, &newServerInfo, CServerInfo_areEqual, (void **)&foundServer);
		switch(result)
		{
			case EPHIDGET_OK: //Found

				if((result = CList_removeFromList((CListHandle *)((long)foundServer + (long)objectListOffset), phid, compareObjects, PFALSE, NULL)))
					return result;
				
				//stop reports for this Object
				if(phid->networkInfo->listen_id)
				{
					CThread_mutex_lock(&phid->networkInfo->server->pdc_lock);
					if(!pdc_ignore(foundServer->server->pdcs,phid->networkInfo->listen_id,errdesc,sizeof(errdesc)))
					{
						LOG(PHIDGET_LOG_DEBUG,"pdc_ignore: %s", errdesc);
					}
					CThread_mutex_unlock(&phid->networkInfo->server->pdc_lock);
				}

				//no more references to this server
				closeServer(foundServer);

				phid->networkInfo->server = NULL;
				
				break;
			case EPHIDGET_NOTFOUND: //Not Found - That's ok, just means it's already closed
				return EPHIDGET_OK;
			default:
				return result;
		}
	}

	return EPHIDGET_OK;
}

CThread_func_return_t DisconnectPhidgetThreadFunction(CThread_func_arg_t lpdwParam)
{
	CPhidgetHandle phid = (CPhidgetHandle)lpdwParam;
	if(phid->fptrDisconnect)
		phid->fptrDisconnect((CPhidgetHandle)phid, phid->fptrDisconnectptr);
	CThread_mutex_lock(&serverLock);
	disconnectRemoteObject(phid, offsetof(CServerInfo, phidgets), CPhidget_areEqual);
	CThread_mutex_unlock(&serverLock);
	return EPHIDGET_OK;
}

int unregisterRemotePhidget(CPhidgetHandle phid)
{
	int result = EPHIDGET_OK;

	CThread_mutex_lock(&activeRemotePhidgetsLock);
	
	result = CList_removeFromList((CListHandle *)&activeRemotePhidgets, phid, CPhidgetHandle_areEqual, FALSE, NULL);

	if (result)
	{
		CThread_mutex_unlock(&activeRemotePhidgetsLock);
		return result;
	}
	CThread_mutex_unlock(&activeRemotePhidgetsLock);

	result = disconnectRemoteObject(phid, offsetof(CServerInfo, phidgets), CPhidget_areEqual);
	CPhidgetRemote_free(phid->networkInfo);
	phid->networkInfo = NULL;

	if(!activeRemotePhidgets && !activeRemoteManagers && !activeRemoteDictionaries)
	{
		JoinCentralRemoteThread();
	}

	return result;
}

int unregisterRemoteManager(CPhidgetManagerHandle phidm)
{
	int result = EPHIDGET_OK;
	CServerInfoHandle foundServer;
	char errdesc[1024];
	CServerInfo newServerInfo;
		
		
	CThread_mutex_lock(&activeRemoteManagersLock);
	
	result = CList_removeFromList((CListHandle *)&activeRemoteManagers, phidm, CPhidgetHandle_areEqual, FALSE, NULL);

	if (result)
	{
		CThread_mutex_unlock(&activeRemoteManagersLock);
		return result;
	}
	CThread_mutex_unlock(&activeRemoteManagersLock);
		
	if(phidm->networkInfo->server)
	{
		newServerInfo.server = phidm->networkInfo->server;

		//check to see if there is already a connection to this server
		result = CList_findInList((CListHandle)servers, &newServerInfo, CServerInfo_areEqual, (void **)&foundServer);
		switch(result)
		{
			case EPHIDGET_OK: //Found

				if((result = CList_removeFromList((CListHandle *)&foundServer->managers, phidm, CPhidgetManager_areEqual, PFALSE, NULL)))
					return result;

				//stop reports
				CThread_mutex_lock(&phidm->networkInfo->server->pdc_lock);
				if(!pdc_ignore(foundServer->server->pdcs,phidm->networkInfo->listen_id,errdesc,sizeof(errdesc)))
				{
					LOG(PHIDGET_LOG_DEBUG,"pdc_ignore: %s", errdesc);
				}
				CThread_mutex_unlock(&phidm->networkInfo->server->pdc_lock);
				
				closeServer(foundServer);

				phidm->networkInfo->server = NULL;
				CPhidgetRemote_free(phidm->networkInfo);
				phidm->networkInfo = NULL;
				
				break;
			case EPHIDGET_NOTFOUND: //Not Found - That's ok, just means it's already closed
				return EPHIDGET_OK;
			default:
				return result;
		}
	}

	if(!activeRemotePhidgets && !activeRemoteManagers && !activeRemoteDictionaries)
	{
		JoinCentralRemoteThread();
	}

	return EPHIDGET_OK;
}

int unregisterRemoteDictionary(CPhidgetDictionaryHandle dict)
{
	int result = EPHIDGET_OK;
	CServerInfoHandle foundServer;
	char errdesc[1024];
	CServerInfo newServerInfo;
	CPhidgetDictionaryListenerListHandle trav;
	
	CThread_mutex_lock(&activeRemoteDictionariesLock);
	
	result = CList_removeFromList((CListHandle *)&activeRemoteDictionaries, dict, CPhidgetHandle_areEqual, FALSE, NULL);

	if (result)
	{
		CThread_mutex_unlock(&activeRemoteDictionariesLock);
		return result;
	}
	CThread_mutex_unlock(&activeRemoteDictionariesLock);
		
	if(dict->networkInfo->server)
	{
		newServerInfo.server = dict->networkInfo->server;

		//check to see if there is already a connection to this server
		result = CList_findInList((CListHandle)servers, &newServerInfo, CServerInfo_areEqual, (void **)&foundServer);
		switch(result)
		{
			case EPHIDGET_OK: //Found

				if((result = CList_removeFromList((CListHandle *)&foundServer->dictionaries, dict, CPhidgetDictionary_areEqual, PFALSE, NULL)))
					return result;

				//stop reports
				for(trav = dict->listeners; trav; trav = trav->next)
				{
					CThread_mutex_lock(&dict->networkInfo->server->pdc_lock);
					if(!pdc_ignore(foundServer->server->pdcs,trav->listener->listen_id,errdesc,sizeof(errdesc)))
					{
						LOG(PHIDGET_LOG_DEBUG,"pdc_ignore: %s", errdesc);
					}
					CThread_mutex_unlock(&dict->networkInfo->server->pdc_lock);
				}
				
				closeServer(foundServer);

				//if server needs to be freed it will already have been
				dict->networkInfo->server = NULL;
				CPhidgetRemote_free(dict->networkInfo);
				dict->networkInfo = NULL;
				
				break;
			case EPHIDGET_NOTFOUND: //Not Found - That's ok, just means it's already closed
				return EPHIDGET_OK;
			default:
				return result;
		}
	}

	if(!activeRemotePhidgets && !activeRemoteManagers && !activeRemoteDictionaries)
	{
		JoinCentralRemoteThread();
	}

	return EPHIDGET_OK;
}

int CCONV
CPhidget_openRemoteIP(CPhidgetHandle phid, int serialNumber, const char *address,
    int port, const char *password)
{
	int result = EPHIDGET_OK;
	char portString[6];

	TESTPTR(phid)

	if (serialNumber < -1)
		return EPHIDGET_INVALIDARG;
	
	CThread_mutex_lock(&phid->openCloseLock);
	if (CPhidget_statusFlagIsSet(phid, PHIDGET_OPENED_FLAG))
	{
		LOG(PHIDGET_LOG_WARNING, "Open was called on an already opened Phidget handle.");
		CThread_mutex_unlock(&phid->openCloseLock);
		return EPHIDGET_UNEXPECTED;
	}

	if (serialNumber == -1)
		phid->specificDevice = FALSE;
	else
		phid->specificDevice = TRUE;
	phid->serialNumber = serialNumber;

	if((result = CPhidgetRemote_create(&phid->networkInfo)))
	{
		CThread_mutex_unlock(&phid->openCloseLock);
		return result;
	}

	if(password) {
		if (strlen(password) > 255)
		{
			CThread_mutex_unlock(&phid->openCloseLock);
			return EPHIDGET_INVALIDARG;
		}
		if (!(phid->networkInfo->password = strdup(password)))
		{
			CThread_mutex_unlock(&phid->openCloseLock);
			return EPHIDGET_NOMEMORY;
		}
	}

	snprintf(portString, sizeof(portString), "%d", port);
	if(!(phid->networkInfo->requested_port = strdup(portString)))
	{
		CThread_mutex_unlock(&phid->openCloseLock);
		return EPHIDGET_NOMEMORY;
	}
	if(!(phid->networkInfo->requested_address = strdup(address)))
	{
		CThread_mutex_unlock(&phid->openCloseLock);
		return EPHIDGET_NOMEMORY;
	}
	
	initialize_locks();

	result = RegisterRemotePhidget(phid);

	CPhidget_setStatusFlag(phid, PHIDGET_OPENED_FLAG);
	CThread_mutex_unlock(&phid->openCloseLock);

	return result;
}

int CCONV CPhidget_openRemote(CPhidgetHandle phid, int serialNumber, const char *serverID, const char *password)
{
#ifdef USE_ZEROCONF
	int result = EPHIDGET_OK;
	
	initialize_locks();

	if(InitializeZeroconf()) return EPHIDGET_UNSUPPORTED;

	TESTPTR(phid)

	if (serialNumber < -1)
		return EPHIDGET_INVALIDARG;
	
	CThread_mutex_lock(&phid->openCloseLock);
	if (CPhidget_statusFlagIsSet(phid, PHIDGET_OPENED_FLAG))
	{
		LOG(PHIDGET_LOG_WARNING, "Open was called on an already opened Phidget handle.");
		CThread_mutex_unlock(&phid->openCloseLock);
		return EPHIDGET_UNEXPECTED;
	}

	if (serialNumber == -1)
		phid->specificDevice = FALSE;
	else
		phid->specificDevice = TRUE;
	phid->serialNumber = serialNumber;

	if((result = CPhidgetRemote_create(&phid->networkInfo)))
	{
		CThread_mutex_unlock(&phid->openCloseLock);
		return result;
	}

	if(password) {
		if (strlen(password) > 255)
		{
			CThread_mutex_unlock(&phid->openCloseLock);
			return EPHIDGET_INVALIDARG;
		}
		if (!(phid->networkInfo->password = strdup(password)))
		{
			CThread_mutex_unlock(&phid->openCloseLock);
			return EPHIDGET_NOMEMORY;
		}
	}
	if(serverID)
	{
		if (!(phid->networkInfo->requested_serverID = strdup(serverID)))
		{
			CThread_mutex_unlock(&phid->openCloseLock);
			return EPHIDGET_NOMEMORY;
		}
	}
	
	phid->networkInfo->mdns = PTRUE;

	CThread_mutex_lock(&serverLock);
	result = RegisterRemotePhidget(phid);
	CThread_mutex_unlock(&serverLock);

	CPhidget_setStatusFlag(phid, PHIDGET_OPENED_FLAG);
	CThread_mutex_unlock(&phid->openCloseLock);
	
	return result;
#else
	return EPHIDGET_UNSUPPORTED;
#endif
}

int CCONV CPhidgetManager_openRemoteIP(CPhidgetManagerHandle phidm, const char *address,
    int port, const char *password)
{
	int result = EPHIDGET_OK;
	char portString[6];

	TESTPTR(phidm)

	if((result = CPhidgetRemote_create(&phidm->networkInfo)))
		return result;

	if(password) {
		if (strlen(password) > 255)
			return EPHIDGET_INVALIDARG;
		if (!(phidm->networkInfo->password = strdup(password)))
			return EPHIDGET_NOMEMORY;
	}

	snprintf(portString, sizeof(portString), "%d", port);
	if(!(phidm->networkInfo->requested_port = strdup(portString)))
		return EPHIDGET_NOMEMORY;
	if(!(phidm->networkInfo->requested_address = strdup(address)))
		return EPHIDGET_NOMEMORY;
	
	phidm->state = PHIDGETMANAGER_ACTIVE;
	
	initialize_locks();

	CThread_mutex_lock(&serverLock);
	result = RegisterRemoteManager(phidm);
	CThread_mutex_unlock(&serverLock);

	return result;
}

int CCONV CPhidgetManager_openRemote(CPhidgetManagerHandle phidm, const char *serverID, const char *password)
{
#ifdef USE_ZEROCONF
	int result = EPHIDGET_OK;
	
	initialize_locks();

	if(InitializeZeroconf()) return EPHIDGET_UNSUPPORTED;

	if((result = CPhidgetRemote_create(&phidm->networkInfo)))
		return result;

	if(password) {
		if (strlen(password) > 255)
			return EPHIDGET_INVALIDARG;
		if (!(phidm->networkInfo->password = strdup(password)))
			return EPHIDGET_NOMEMORY;
	}
	if(serverID)
	{
		if (!(phidm->networkInfo->requested_serverID = strdup(serverID)))
			return EPHIDGET_NOMEMORY;
	}

	phidm->networkInfo->mdns = PTRUE;

	//we're not connecting to anything, so - always active :)
	phidm->state = PHIDGETMANAGER_ACTIVE;

	result = RegisterRemoteManager(phidm);
	if(!result)
	{
		CPhidgetListHandle trav;
		CThread_mutex_lock(&zeroconfPhidgetsLock);
		CThread_mutex_lock(&activeRemoteManagersLock);
		for (trav=zeroconfPhidgets; trav; trav = trav->next)
		{
				if (phidm->fptrAttachChange)
					phidm->fptrAttachChange((CPhidgetHandle)trav->phid, phidm->fptrAttachChangeptr);
		}
		CThread_mutex_unlock(&activeRemoteManagersLock);
		CThread_mutex_unlock(&zeroconfPhidgetsLock);
	}

	return result;
#else
	return EPHIDGET_UNSUPPORTED;
#endif
}

int CCONV CPhidgetDictionary_openRemoteIP(CPhidgetDictionaryHandle dict, const char *address,
    int port, const char *password)
{
	int result = EPHIDGET_OK;
	char portString[6];
	TESTPTR(dict)

	if((result = CPhidgetRemote_create(&dict->networkInfo)))
		return result;

	snprintf(portString, sizeof(portString), "%d", port);
	if(!(dict->networkInfo->requested_port = strdup(portString)))
		return EPHIDGET_NOMEMORY;
	if(!(dict->networkInfo->requested_address = strdup(address)))
		return EPHIDGET_NOMEMORY;

	if(password) {
		if (strlen(password) > 255)
			return EPHIDGET_INVALIDARG;
		if (!(dict->networkInfo->password = strdup(password)))
			return EPHIDGET_NOMEMORY;
	}
	
	initialize_locks();

	CThread_mutex_lock(&serverLock);
	result = RegisterRemoteDictionary(dict);
	CThread_mutex_unlock(&serverLock);

	return result;
}

int CCONV CPhidgetDictionary_openRemote(CPhidgetDictionaryHandle dict, const char *serverID, const char *password)
{
#ifdef USE_ZEROCONF
	int result = EPHIDGET_OK;
	
	initialize_locks();

	if(InitializeZeroconf()) return EPHIDGET_UNSUPPORTED;

	if((result = CPhidgetRemote_create(&dict->networkInfo)))
		return result;

	if(password) {
		if (strlen(password) > 255)
			return EPHIDGET_INVALIDARG;
		if (!(dict->networkInfo->password = strdup(password)))
			return EPHIDGET_NOMEMORY;
	}
	if(serverID)
	{
		if (!(dict->networkInfo->requested_serverID = strdup(serverID)))
			return EPHIDGET_NOMEMORY;
	}

	dict->networkInfo->mdns = PTRUE;

	CThread_mutex_lock(&serverLock);
	result = RegisterRemoteDictionary(dict);
	CThread_mutex_unlock(&serverLock);

	return result;
#else
	return EPHIDGET_UNSUPPORTED;
#endif
}
