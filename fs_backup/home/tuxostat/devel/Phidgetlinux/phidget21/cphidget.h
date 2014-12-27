#ifndef __CPHIDGET
#define __CPHIDGET

/** \defgroup phidcommon Phidget Common
 * Calls common to all Phidgets. See the programming manual for more specific API details, supported functionality, units, etc.
 * @{
 */
 
/**
 * A phidget handle.
 */
typedef struct _CPhidget *CPhidgetHandle;
#ifndef _WINDOWS
/**
 * Platform independent 64-bit integer.
 */
typedef long long __int64;
#endif

#ifndef EXTERNALPROTO
#include "cphidgetattr.h"
#include "cphidgetconstantsinternal.h"
#include "cphidgetconstants.h"
#include "cthread.h"

typedef struct _CPhidget CPhidget;
//This is for a socket - an one of these can be referenced by several phidgets!
typedef struct __CPhidgetSocketClient {
	int socket;
	char *port;
	char *address;
	void *pdcs;
	int status;
	CThread_mutex_t lock; /* protects server status */
	CThread_mutex_t pdc_lock; /* protects synchronous pdc functions */
} CPhidgetSocketClient, *CPhidgetSocketClientHandle;

typedef struct __CPhidgetSocketClientList
{
	struct __CPhidgetSocketClientList *next;
	CPhidgetSocketClientHandle client;
} CPhidgetSocketClientList, *CPhidgetSocketClientListHandle;

typedef struct __CPhidgetRemote {
	CPhidgetSocketClientHandle server;	//can be NULL if not yet connected to a server
	char *requested_port;		//only !NULL if not zeroconf
	char *requested_address;	//only !NULL if not zeroconf
	char *requested_serverID;	//only !NULL if zeroconf
	char *password;				//can be NULL - will be used if needed for connecting to remote Phidgets
	int listen_id;
	int mdns;					//true if mdns, false if not
	char *zeroconf_name;		//for use before a connection is active
	char *zeroconf_domain;		//for use before a connection is active
	char *zeroconf_type;		//for use before a connection is active
	char *zeroconf_host;
	char *zeroconf_port;
	char *zeroconf_server_id;	//for use before a connection is active
	int zeroconf_auth;			//for use before a connection is active
} CPhidgetRemote, *CPhidgetRemoteHandle;

typedef struct __CPhidgetRemoteList
{
	struct __CPhidgetRemoteList *next;
	CPhidgetRemoteHandle networkInfo;
} CPhidgetRemoteList, *CPhidgetRemoteListHandle;

struct _CPhidget {
	CPhidgetRemoteHandle networkInfo; //NULL if local, !NULL if remote
	int(CCONV *fptrError)(CPhidgetHandle , void *, int, const char *);
	void *fptrErrorptr;
	int(CCONV *fptrConnect)(CPhidgetHandle , void *);
	void *fptrConnectptr; 
	int(CCONV *fptrDisconnect)(CPhidgetHandle , void *);
	void *fptrDisconnectptr;
	CThread_mutex_t lock; /* protects status */
	int status;
	CThread_mutex_t openCloseLock; /* protects status */
	int keyCount; //counts key during network open
	CThread_mutex_t writelock; /* protects write - exclusive */
	CThread readThread;
	CThread writeThread;
	HANDLE deviceHandle;
#ifdef _WINDOWS
	OVERLAPPED asyncRead;
	BOOL readPending;
	EVENT closeReadEvent;
	OVERLAPPED asyncWrite;
	unsigned char inbuf[MAX_IN_PACKET_SIZE+1];
#endif
	int specificDevice;
	unsigned short deviceID;
	unsigned short deviceIDSpec;
	int deviceVersion;
	unsigned short ProductID;
	unsigned short VendorID;
	int serialNumber;
	const char *deviceType;
	unsigned short outputReportByteLength;
	unsigned short inputReportByteLength;
	char label[11];
	int(CCONV *fptrInit)(CPhidgetHandle);
	int(CCONV *fptrClear)(CPhidgetHandle);
	int(CCONV *fptrEvents)(CPhidgetHandle);
	int(CCONV *fptrData)(CPhidgetHandle, unsigned char *buffer, int length);
	int(CCONV *fptrGetPacket)(CPhidgetHandle, unsigned char *buffer,
	  unsigned int *length);
	unsigned char lastReadPacket[MAX_IN_PACKET_SIZE];
	unsigned char awdc_enabled;
	void *dnsServiceRef;
	void *CPhidgetFHandle;
	CThread_mutex_t outputLock; /* device-specific code responsible */
	EVENT writeAvailableEvent; /* device-specific code sets w/o OLL held */
	EVENT writtenEvent; /* device-specific code clears w/OLL held */
	int writeStopFlag; /* set when closing */
	int(CCONV *fptrAttach)(CPhidgetHandle , void *);
	void *fptrAttachptr; 
	int(CCONV *fptrDetach)(CPhidgetHandle , void *);
	void *fptrDetachptr;
	CPhidgetAttr attr;
};

struct _CPhidgetList
{
	struct _CPhidgetList *next;
	CPhidgetHandle phid;
} typedef CPhidgetList, *CPhidgetListHandle;

typedef struct {
	int pdd_did;
	int pdd_vid;
	int pdd_pid;
	CPhidgetAttr pdd_attr;
	int pdd_iid;
} CPhidgetDeviceDef;

extern const char LibraryVersion[];
extern const char *Phid_DeviceName[PHIDGET_DEVICE_TYPE_COUNT];
extern const char *Phid_DeviceSpecificName[PHIDGET_DEVICE_COUNT];
extern const char Phid_Device_Def_WorkAroundA[PHIDGET_DEVICE_COUNT][2];
extern const char Phid_UnknownErrorDescription[];
extern const char *Phid_ErrorDescriptions[PHIDGET_ERROR_CODE_COUNT];

extern CPhidgetList *ActiveDevices;
extern CPhidgetList *AttachedDevices;
extern int phidgetLocksInitialized;
extern CThread_mutex_t activeDevicesLock, attachedDevicesLock;

extern const CPhidgetDeviceDef Phid_Device_Def[];

int CPhidget_read(CPhidgetHandle phid);
int CPhidget_write(CPhidgetHandle phid);
int CPhidget_statusFlagIsSet(CPhidgetHandle phid, int flag);
int CPhidget_setStatusFlag(CPhidgetHandle phid, int flag);
int CPhidget_clearStatusFlag(CPhidgetHandle phid, int flag);
char translate_bool_to_ascii(char value);
const char *CPhidget_strerror(int error);
void throw_error_event(CPhidgetHandle phid, const char *error, int errcode);
int attachActiveDevice(CPhidgetHandle activeDevice, CPhidgetHandle attachedDevice);

PHIDGET21_API int CCONV CPhidget_areEqual(void *arg1, void *arg2);
PHIDGET21_API int CCONV CPhidget_areExtraEqual(void *arg1, void *arg2);
PHIDGET21_API int CCONV CPhidgetHandle_areEqual(void *arg1, void *arg2);
PHIDGET21_API void CCONV CPhidget_free(void *arg);
PHIDGET21_API int CCONV phidget_type_to_id(const char *Type);
PHIDGET21_API int CCONV CPhidget_create(CPhidgetHandle *phid);

PHIDGET21_API int CCONV CPhidget_calibrate_gainoffset(CPhidgetHandle phid, int index, unsigned short offset, unsigned long gain);

#endif
#include "cphidgetmacros.h"

/**
 * Opens a Phidget.
 * @param phid A phidget handle.
 * @param serialNumber Serial number. Specify -1 to open any.
 */	
PHIDGET21_API int CCONV CPhidget_open(CPhidgetHandle phid, int serialNumber);
/**
 * Closes a Phidget.
 * @param phid An opened phidget handle.
 */
PHIDGET21_API int CCONV CPhidget_close(CPhidgetHandle phid);
/**
 * Frees a Phidget handle.
 * @param phid A closed phidget handle.
 */
PHIDGET21_API int CCONV CPhidget_delete(CPhidgetHandle phid);
/**
 * Sets a detach handler callback function. This is called when this Phidget is unplugged from the system.
 * @param phid A phidget handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
PHIDGET21_API int CCONV CPhidget_set_OnDetach_Handler(CPhidgetHandle phid, int(CCONV *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
/**
 * Sets an attach handler callback function. This is called when this Phidget is plugged into the system, and is ready for use.
 * @param phid A phidget handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
PHIDGET21_API int CCONV CPhidget_set_OnAttach_Handler(CPhidgetHandle phid, int(CCONV *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
/**
 * Sets a server connect handler callback function. This is used for opening Phidgets remotely, and is called when a connection to the sever has been made.
 * @param phid A phidget handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
PHIDGET21_API int CCONV CPhidget_set_OnServerConnect_Handler(CPhidgetHandle phid, int (CCONV *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
/**
 * Sets a server disconnect handler callback function. This is used for opening Phidgets remotely, and is called when a connection to the server has been lost.
 * @param phid A phidget handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
PHIDGET21_API int CCONV CPhidget_set_OnServerDisconnect_Handler(CPhidgetHandle phid, int (CCONV *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
/**
 * Sets the error handler callback function. This is called when an asynchronous error occurs.
 * @param phid A phidget handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
PHIDGET21_API int CCONV CPhidget_set_OnError_Handler(CPhidgetHandle phid, int(CCONV *fptr)(CPhidgetHandle phid, void *userPtr, int errorCode, const char *errorString), void *userPtr);
/**
 * Gets the specific name of a Phidget.
 * @param phid An attached phidget handle.
 * @param deviceName A pointer which will be set to point to a char array containing the device name.
 */
PHIDGET21_API int CCONV CPhidget_getDeviceName(CPhidgetHandle phid, const char **deviceName);
/**
 * Gets the serial number of a Phidget.
 * @param phid An attached phidget handle.
 * @param serialNumber An int pointer for returning the serial number.
 */
PHIDGET21_API int CCONV CPhidget_getSerialNumber(CPhidgetHandle phid, int *serialNumber);
/**
 * Gets the firmware version of a Phidget.
 * @param phid An attached phidget handle.
 * @param deviceVersion An int pointer for returning the device version.
 */
PHIDGET21_API int CCONV CPhidget_getDeviceVersion(CPhidgetHandle phid, int *deviceVersion);
/**
 * Gets the attached status of a Phidget.
 * @param phid A phidget handle.
 * @param deviceStatus An int pointer for returning the device status. Possible codes are \ref PHIDGET_ATTACHED and \ref PHIDGET_NOTATTACHED.
 */
PHIDGET21_API int CCONV CPhidget_getDeviceStatus(CPhidgetHandle phid, int *deviceStatus);
/**
 * Gets the library version. This contains a version number and a build date.
 * @param libraryVersion A pointer which will be set to point to a char array containing the library version string.
 */
PHIDGET21_API int CCONV CPhidget_getLibraryVersion(const char **libraryVersion);
/**
 * Gets the type (class) of a Phidget.
 * @param phid An attached phidget handle.
 * @param deviceType A pointer which will be set to a char array containing the device type string.
 */
PHIDGET21_API int CCONV CPhidget_getDeviceType(CPhidgetHandle phid, const char **deviceType);
/**
 * Gets the label of a Phidget.
 * @param phid An attached phidget handle.
 * @param deviceLabel A pointer which will be set to a char array containing the device label string.
 */
PHIDGET21_API int CCONV CPhidget_getDeviceLabel(CPhidgetHandle phid, const char **deviceLabel);
/**
 * Sets the label of a Phidget. Note that this is nut supported on very old Phidgets, and not yet supported in Windows.
 * @param phid An attached phidget handle.
 * @param deviceLabel A string containing the label to be set.
 */
PHIDGET21_API int CCONV CPhidget_setDeviceLabel(CPhidgetHandle phid, const char *deviceLabel);
/**
 * Gets the description for an error code.
 * @param errorCode The error code to get the description of.
 * @param errorString A pointer which will be set to a char array containing the error description string.
 */
PHIDGET21_API int CCONV CPhidget_getErrorDescription(int errorCode, const char **errorString);
/**
 * Waits for attachment to happen. This can be called wirght after calling \ref CPhidget_open, as an alternative to using the attach handler.
 * @param phid An opened phidget handle.
 * @param milliseconds Time to wait for the attachment. Specify 0 to wait forever.
 */
PHIDGET21_API int CCONV CPhidget_waitForAttachment(CPhidgetHandle phid, int milliseconds);
/**
 * Gets the server ID of a remotely opened Phidget. This will fail if the Phidget was opened locally.
 * @param phid A connected phidget handle.
 * @param serverID A pointer which will be set to a char array containing the server ID string.
 */
PHIDGET21_API int CCONV CPhidget_getServerID(CPhidgetHandle phid, const char **serverID);
/**
 * Gets the address and port of a remotely opened Phidget. This will fail if the Phidget was opened locally.
 * @param phid A connected phidget handle.
 * @param address A pointer which will be set to a char array containing the address string.
 * @param port An int pointer for returning the port number.
 */
PHIDGET21_API int CCONV CPhidget_getServerAddress(CPhidgetHandle phid, const char **address, int *port);
/**
 * Gets the connected to server status of a remotely opened Phidget. This will fail if the Phidget was opened locally.
 * @param phid An opened phidget handle.
 * @param serverStatus An int pointer for returning the server status. Possible codes are \ref PHIDGET_ATTACHED and \ref PHIDGET_NOTATTACHED.
 */
PHIDGET21_API int CCONV CPhidget_getServerStatus(CPhidgetHandle phid, int *serverStatus);

/** @} */

#endif
