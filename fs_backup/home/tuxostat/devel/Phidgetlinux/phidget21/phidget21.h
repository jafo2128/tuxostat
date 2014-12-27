#ifndef PHIDGET_H
#define PHIDGET_H
#ifdef __cplusplus
extern "C" {
#endif





 /*! \mainpage Phidget21 C API Documentation
 *
 * Describes the Application Program Interface (API) for each Phidget device.  
 * The API can be used by a number of languages; 
 * this manual discusses use via C and the code examples reflect this.
 *
 * \section howto_sec How to use Phidgets
 Phidgets are an easy to use set of building blocks for low cost sensing and control from your PC.  
 Using the Universal Serial Bus (USB) as the basis for all Phidgets, the complexity is managed behind 
 this easy to use and robust Application Program Interface (API) library.
 
 The library was written originally for Windows, but has been ported to MacOS and Linux.  
 Although the library is written in C, the functions can be called from a number of languages including C, C++, Objective-C, Matlab, etc.  
 The source code is freely available for reference/debugging purposes.
 
 See the <a class="el" href="modules.html">Modules</a> section for the API documentation.
 
 See \ref phidcommon for calls common to all phidgets.

 Refer to the <a class="el" href="http://www.phidgets.com/information.php#products" target="_blank">Product manual</a> for your Phidget and the 
 <a class="el" href="http://www.phidgets.com/documentation/Programming_Manual.pdf" target="_blank">Programming Manual</a> for more detailed, language inspecific API documentation. 
 Also, there are a set of C/C++ examples available for download.

 \section general_sec General Usage
Phidgets in C are accessed/controlled via handles. Every Phidget has it's own Handle, as well as a generic Phidget handle. These handles are simply pointers
from the user's perspective - internally, they point to structures. Almost every function in the API takes a Phidget handle as the first argument.

phidget21.h contains the API calls for all Phidgets. It should be included in any programs which need to access the Phidget C Library from C/C++.

Note that this file is not distributed in the source, this file is generated from the source using the C preproccessor. It is installed with the libraries.

The first step is to create the handle. Every phidget has it's own _create function. This creates space for the structure internally and gives you a 
valid Handle for passing to all other Phidget functions.

The next step is to register event handlers. These need to be registered now, because they only throw events in the future, 
and if they are registered later, you will miss events. Every Phidget event takes a Phidget handle, a function callback pointer and
a user defined pointer, which is passed back from within the callback.

The next step is to open the Phidget. This is done with either \ref CPhidget_open, \ref CPhidget_openRemote or \ref CPhidget_openRemoteIP, depending on whether you want
to open the Phidget locally or over the network. Open returns right away (is asynchronous) so you have to either wait for the attach event, 
or call \ref CPhidget_waitForAttachment before using the handle further.

The next step it to actually use the Phidget. Once attached, all functions can be called. Device specific handles should be cast to \ref CPhidgetHandle for use
with the common API functions.

Once finished, you should call first \ref CPhidget_close and then \ref CPhidget_delete to clean up the connection to the Phidget and the Handle before exiting.

\section return_sec Return Codes
Every Phidget function returns an int. A return code of 0 means success, anything else means failure. The failure codes are documented here: \ref phidconst.

When a value is unknown, in addition to returning an error code, the pointer will be set to one of the unknonwn values documented here: \ref phidconst.
 *
 */

/** \defgroup phidgets Specific Phidgets
 */
/** \defgroup phidcommon Phidget Common
 * Calls common to all Phidgets. See the programming manual for more specific API details, supported functionality, units, etc.
 * @{
 */
/**
 * A phidget handle.
 */
typedef struct _CPhidget *CPhidgetHandle;
/**
 * Platform independent 64-bit integer.
 */
typedef long long __int64;
//Regular Versions
//Versions for Deprecation
/**
 * Opens a Phidget.
 * @param phid A phidget handle.
 * @param serialNumber Serial number. Specify -1 to open any.
 */
 int CPhidget_open(CPhidgetHandle phid, int serialNumber);
/**
 * Closes a Phidget.
 * @param phid An opened phidget handle.
 */
 int CPhidget_close(CPhidgetHandle phid);
/**
 * Frees a Phidget handle.
 * @param phid A closed phidget handle.
 */
 int CPhidget_delete(CPhidgetHandle phid);
/**
 * Sets a detach handler callback function. This is called when this Phidget is unplugged from the system.
 * @param phid A phidget handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidget_set_OnDetach_Handler(CPhidgetHandle phid, int( *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
/**
 * Sets an attach handler callback function. This is called when this Phidget is plugged into the system, and is ready for use.
 * @param phid A phidget handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidget_set_OnAttach_Handler(CPhidgetHandle phid, int( *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
/**
 * Sets a server connect handler callback function. This is used for opening Phidgets remotely, and is called when a connection to the sever has been made.
 * @param phid A phidget handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidget_set_OnServerConnect_Handler(CPhidgetHandle phid, int ( *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
/**
 * Sets a server disconnect handler callback function. This is used for opening Phidgets remotely, and is called when a connection to the server has been lost.
 * @param phid A phidget handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidget_set_OnServerDisconnect_Handler(CPhidgetHandle phid, int ( *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
/**
 * Sets the error handler callback function. This is called when an asynchronous error occurs.
 * @param phid A phidget handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidget_set_OnError_Handler(CPhidgetHandle phid, int( *fptr)(CPhidgetHandle phid, void *userPtr, int errorCode, const char *errorString), void *userPtr);
/**
 * Gets the specific name of a Phidget.
 * @param phid An attached phidget handle.
 * @param deviceName A pointer which will be set to point to a char array containing the device name.
 */
 int CPhidget_getDeviceName(CPhidgetHandle phid, const char **deviceName);
/**
 * Gets the serial number of a Phidget.
 * @param phid An attached phidget handle.
 * @param serialNumber An int pointer for returning the serial number.
 */
 int CPhidget_getSerialNumber(CPhidgetHandle phid, int *serialNumber);
/**
 * Gets the firmware version of a Phidget.
 * @param phid An attached phidget handle.
 * @param deviceVersion An int pointer for returning the device version.
 */
 int CPhidget_getDeviceVersion(CPhidgetHandle phid, int *deviceVersion);
/**
 * Gets the attached status of a Phidget.
 * @param phid A phidget handle.
 * @param deviceStatus An int pointer for returning the device status. Possible codes are \ref PHIDGET_ATTACHED and \ref PHIDGET_NOTATTACHED.
 */
 int CPhidget_getDeviceStatus(CPhidgetHandle phid, int *deviceStatus);
/**
 * Gets the library version. This contains a version number and a build date.
 * @param libraryVersion A pointer which will be set to point to a char array containing the library version string.
 */
 int CPhidget_getLibraryVersion(const char **libraryVersion);
/**
 * Gets the type (class) of a Phidget.
 * @param phid An attached phidget handle.
 * @param deviceType A pointer which will be set to a char array containing the device type string.
 */
 int CPhidget_getDeviceType(CPhidgetHandle phid, const char **deviceType);
/**
 * Gets the label of a Phidget.
 * @param phid An attached phidget handle.
 * @param deviceLabel A pointer which will be set to a char array containing the device label string.
 */
 int CPhidget_getDeviceLabel(CPhidgetHandle phid, const char **deviceLabel);
/**
 * Sets the label of a Phidget. Note that this is nut supported on very old Phidgets, and not yet supported in Windows.
 * @param phid An attached phidget handle.
 * @param deviceLabel A string containing the label to be set.
 */
 int CPhidget_setDeviceLabel(CPhidgetHandle phid, const char *deviceLabel);
/**
 * Gets the description for an error code.
 * @param errorCode The error code to get the description of.
 * @param errorString A pointer which will be set to a char array containing the error description string.
 */
 int CPhidget_getErrorDescription(int errorCode, const char **errorString);
/**
 * Waits for attachment to happen. This can be called wirght after calling \ref CPhidget_open, as an alternative to using the attach handler.
 * @param phid An opened phidget handle.
 * @param milliseconds Time to wait for the attachment. Specify 0 to wait forever.
 */
 int CPhidget_waitForAttachment(CPhidgetHandle phid, int milliseconds);
/**
 * Gets the server ID of a remotely opened Phidget. This will fail if the Phidget was opened locally.
 * @param phid A connected phidget handle.
 * @param serverID A pointer which will be set to a char array containing the server ID string.
 */
 int CPhidget_getServerID(CPhidgetHandle phid, const char **serverID);
/**
 * Gets the address and port of a remotely opened Phidget. This will fail if the Phidget was opened locally.
 * @param phid A connected phidget handle.
 * @param address A pointer which will be set to a char array containing the address string.
 * @param port An int pointer for returning the port number.
 */
 int CPhidget_getServerAddress(CPhidgetHandle phid, const char **address, int *port);
/**
 * Gets the connected to server status of a remotely opened Phidget. This will fail if the Phidget was opened locally.
 * @param phid An opened phidget handle.
 * @param serverStatus An int pointer for returning the server status. Possible codes are \ref PHIDGET_ATTACHED and \ref PHIDGET_NOTATTACHED.
 */
 int CPhidget_getServerStatus(CPhidgetHandle phid, int *serverStatus);
/** @} */
/** \defgroup phiddict Phidget Dictionary 
 * Calls specific to the Phidget Dictionary.
 * @{
 */
/**
 * Possible reasons for a key event.
 */
typedef enum {
 PHIDGET_DICTIONARY_VALUE_CHANGED = 1, /**< The value of an existing key/value pair changed. */
 PHIDGET_DICTIONARY_ENTRY_ADDED, /**< A new key/value pair was added. */
 PHIDGET_DICTIONARY_ENTRY_REMOVING, /**< A key is being removed. */
 PHIDGET_DICTIONARY_CURRENT_VALUE /**< Initial state received once a handler was added. */
} CPhidgetDictionary_keyChangeReason;
/**
 * A Phidget Dictionary handle.
 */
typedef struct _CPhidgetDictionary *CPhidgetDictionaryHandle;
/**
 * A Dictionary key listener handle.
 */
typedef struct _CPhidgetDictionaryListener *CPhidgetDictionaryListenerHandle;
/**
 * Creates a Phidget Dictionary handle.
 * @param dict A pointer to an unallocated phidget dictionary handle.
 */
 int CPhidgetDictionary_create(CPhidgetDictionaryHandle *dict);
/**
 * Closes the connection to a Phidget Dictionary.
 * @param dict An opened phidget dictionary handle.
 */
 int CPhidgetDictionary_close(CPhidgetDictionaryHandle dict);
/**
 * Frees a Phidget Dictionary handle.
 * @param dict A closed dictionary handle.
 */
 int CPhidgetDictionary_delete(CPhidgetDictionaryHandle dict);
/**
 * Sets the error handler callback function. This is called when an asynchronous error occurs.
 * @param dict A phidget dictionary handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetDictionary_set_OnError_Handler(CPhidgetDictionaryHandle dict,
    int( *fptr)(CPhidgetDictionaryHandle, void *userPtr, int errorCode, const char *errorString), void *userPtr);
/**
 * Adds a key/value pair to the dictionary. Or, changes an existing key's value.
 * @param dict A connected dictionary handle.
 * @param key The key value.
 * @param value The value value.
 * @param persistent Whether the key stays in the dictionary after disconnection.
 */
 int CPhidgetDictionary_addKey(CPhidgetDictionaryHandle dict, const char *key, const char *value, int persistent);
/**
 * Removes a set of keys from the dictionary.
 * @param dict A connected dictionary handle.
 * @param pattern A regular expression representing th eset of keys to remove.
 */
 int CPhidgetDictionary_removeKey(CPhidgetDictionaryHandle dict, const char *pattern);
/**
 * Callback function for KeyChange events.
 * @param dict Dictionary from which this event originated.
 * @param userPtr User defined data.
 * @param key Key value.
 * @param value Value value.
 * @param reason Reason for KeyChange event.
 */
typedef int( *CPhidgetDictionary_OnKeyChange_Function)(CPhidgetDictionaryHandle dict, void *userPtr,
 const char *key, const char *value, CPhidgetDictionary_keyChangeReason reason);
/**
 * Adds a key listener to an opened dictionary. Note that this should only be called after the connection to the
 * dictionary has been made - unlike all other events.
 * @param dict A connected dictionary handle.
 * @param dictlistener A pointer to an unallocated dictionary key listener handle.
 * @param pattern A regular expression representing the set of keys to monitor.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetDictionary_set_OnKeyChange_Handler(CPhidgetDictionaryHandle dict, CPhidgetDictionaryListenerHandle *dictlistener, const char *pattern,
 CPhidgetDictionary_OnKeyChange_Function fptr, void *userPtr);
/**
 * Removes a key listener.
 * @param dictlistener The dictionary key listener created by \ref CPhidgetDictionary_set_OnKeyChange_Handler
 */
 int CPhidgetDictionary_remove_OnKeyChange_Handler(CPhidgetDictionaryListenerHandle dictlistener);
/**
 * Not Implemented.
 */
 int CPhidgetDictionary_getKey(CPhidgetDictionaryHandle dict, const char *key, const char *value, int valueSize);
/**
 * Sets a server connect handler callback function. This is called when a connection to the sever has been made.
 * @param dict A phidget dictionary handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetDictionary_set_OnServerConnect_Handler(CPhidgetDictionaryHandle dict, int ( *fptr)(CPhidgetDictionaryHandle dict, void *userPtr), void *userPtr);
/**
 * Sets a server disconnect handler callback function. This is called when a connection to the server has been lost.
 * @param dict A phidget dictionary handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetDictionary_set_OnServerDisconnect_Handler(CPhidgetDictionaryHandle dict, int ( *fptr)(CPhidgetDictionaryHandle dict, void *userPtr), void *userPtr);
/**
 * Gets the server ID.
 * @param dict A connected dictionary handle.
 * @param serverID A pointer which will be set to a char array containing the server ID string.
 */
 int CPhidgetDictionary_getServerID(CPhidgetDictionaryHandle dict, const char **serverID);
/**
 * Gets the address and port.
 * @param dict A connected dictionary handle.
 * @param address A pointer which will be set to a char array containing the address string.
 * @param port An int pointer for returning the port number.
 */
 int CPhidgetDictionary_getServerAddress(CPhidgetDictionaryHandle dict, const char **address, int *port);
/**
 * Gets the connected to server status.
 * @param dict An opened dictionary handle.
 * @param serverStatus An int pointer for returning the server status. Possible codes are \ref PHIDGET_ATTACHED and \ref PHIDGET_NOTATTACHED.
 */
 int CPhidgetDictionary_getServerStatus(CPhidgetDictionaryHandle dict, int *serverStatus);
/** @} */
/** \defgroup phidmanager Phidget Manager 
 * Calls specific to the Phidget Manager. The Phidget Manager allows enumeration of all connected devices and notification of device
 * attach and detach events.
 * @{
 */
/**
 * A Phidget Manager handle.
 */
typedef struct _CPhidgetManager *CPhidgetManagerHandle;
/**
 * Creates a Phidget Manager handle.
 * @param phidm A pointer to an empty phidget manager handle.
 */
 int CPhidgetManager_create(CPhidgetManagerHandle *phidm);
/**
 * Opens a Phidget Manager.
 * @param phidm A phidget manager handle.
 */
 int CPhidgetManager_open(CPhidgetManagerHandle phidm);
/**
 * Closes a Phidget Manager.
 * @param phidm An opened phidget manager handle.
 */
 int CPhidgetManager_close(CPhidgetManagerHandle phidm);
/**
 * Frees a Phidget Manager handle.
 * @param phidm A closed phidget manager handle.
 */
 int CPhidgetManager_delete(CPhidgetManagerHandle phidm);
/**
 * Sets an attach handler callback function. This is called when a Phidget is plugged into the system.
 * @param phidm A phidget manager handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetManager_set_OnAttach_Handler(CPhidgetManagerHandle phidm, int ( *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
/**
 * Sets a detach handler callback function. This is called when a Phidget is unplugged from the system.
 * @param phidm A phidget manager handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetManager_set_OnDetach_Handler(CPhidgetManagerHandle phidm, int ( *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
/**
 * Gets a list of all currently attached Phidgets.
 * @param phidm An opened phidget manager handle.
 * @param phidArray An empty pointer for returning the list of Phidgets. Note that this list is created internally, you don't need to pass in a arry.
 * @param count An int pointer for returning the list size
 */
 int CPhidgetManager_getAttachedDevices(CPhidgetManagerHandle phidm, CPhidgetHandle *phidArray[], int *count);
/**
 * Sets the error handler callback function. This is called when an asynchronous error occurs.
 * @param phidm A phidget manager handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetManager_set_OnError_Handler(CPhidgetManagerHandle phidm, int( *fptr)(CPhidgetManagerHandle phidm, void *userPtr, int errorCode, const char *errorString), void *userPtr);
/**
 * Sets a server connect handler callback function. This is used for opening Phidget Managers remotely, and is called when a connection to the sever has been made.
 * @param phidm A phidget manager handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetManager_set_OnServerConnect_Handler(CPhidgetManagerHandle phidm, int ( *fptr)(CPhidgetManagerHandle phidm, void *userPtr), void *userPtr);
/**
 * Sets a server disconnect handler callback function. This is used for opening Phidget Managers remotely, and is called when a connection to the server has been lost.
 * @param phidm A phidget manager handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetManager_set_OnServerDisconnect_Handler(CPhidgetManagerHandle phidm, int ( *fptr)(CPhidgetManagerHandle phidm, void *userPtr), void *userPtr);
/**
 * Gets the server ID of a remotely opened Phidget Manager. This will fail if the manager was opened locally.
 * @param phidm A connected phidget manager handle.
 * @param serverID A pointer which will be set to a char array containing the server ID string.
 */
 int CPhidgetManager_getServerID(CPhidgetManagerHandle phidm, const char **serverID);
/**
 * Gets the address and port of a remotely opened Phidget Manager. This will fail if the manager was opened locally.
 * @param phidm A connected phidget manager handle.
 * @param address A pointer which will be set to a char array containing the address string.
 * @param port An int pointer for returning the port number.
 */
 int CPhidgetManager_getServerAddress(CPhidgetManagerHandle phidm, const char **address, int *port);
/**
 * Gets the connected to server status of a remotely opened Phidget Manager. This will fail if the manager was opened locally.
 * @param phidm An opened phidget manager handle.
 * @param serverStatus An int pointer for returning the server status. Possible codes are \ref PHIDGET_ATTACHED and \ref PHIDGET_NOTATTACHED.
 */
 int CPhidgetManager_getServerStatus(CPhidgetManagerHandle phidm, int *serverStatus);
/** @} */
/** \addtogroup phidcommon
 * @{
 */
/**
 * Opens a Phidget remotely by ServerID. Note that this requires Bonjour (mDNS) to be running on both the host and the server.
 * @param phid A phidget handle.
 * @param serial Serial number. Specify -1 to open any.
 * @param serverID Server ID. Specify NULL to open any.
 * @param password Password. Can be NULL if the server is running unsecured.
 */
 int CPhidget_openRemote(CPhidgetHandle phid, int serial, const char *serverID, const char *password);
/**
 * Opens a Phidget remotely by address and port.
 * @param phid A phidget handle.
 * @param serial Serial number. Specify -1 to open any.
 * @param address Address. This can be a hostname or IP address.
 * @param port Port number. Default is 5001.
 * @param password Password. Can be NULL if the server is running unsecured.
 */
 int CPhidget_openRemoteIP(CPhidgetHandle phid, int serial, const char *address, int port, const char *password);
/** @} */
/** \addtogroup phidmanager
 * @{
 */
/**
 * Opens a Phidget manager remotely by ServerID. Note that this requires Bonjour (mDNS) to be running on both the host and the server.
 * @param phidm A phidget manager handle.
 * @param serverID Server ID. Specify NULL to open any.
 * @param password Password. Can be NULL if the server is running unsecured.
 */
 int CPhidgetManager_openRemote(CPhidgetManagerHandle phidm, const char *serverID, const char *password);
/**
 * Opens a Phidget manager remotely by address and port.
 * @param phidm A phidget manager handle.
 * @param address Address. This can be a hostname or IP address.
 * @param port Port number. Default is 5001.
 * @param password Password. Can be NULL if the server is running unsecured.
 */
 int CPhidgetManager_openRemoteIP(CPhidgetManagerHandle phidm, const char *address, int port, const char *password);
/** @} */
/** \addtogroup phiddict
 * @{
 */
/**
 * Opens a Phidget dictionary by ServerID. Note that this requires Bonjour (mDNS) to be running on both the host and the server.
 * @param dict A phidget dictionary handle.
 * @param serverID Server ID. Specify NULL to open any.
 * @param password Password. Can be NULL if the server is running unsecured.
 */
 int CPhidgetDictionary_openRemote(CPhidgetDictionaryHandle dict, const char *serverID, const char *password);
/**
 * Opens a Phidget dictionary by address and port.
 * @param dict A phidget dictionary handle.
 * @param address Address. This can be a hostname or IP address.
 * @param port Port number. Default is 5001.
 * @param password Password. Can be NULL if the server is running unsecured.
 */
 int CPhidgetDictionary_openRemoteIP(CPhidgetDictionaryHandle dict, const char *address, int port, const char *password);
/** @} */
/** \defgroup phidlog Phidget Logging 
 * Logging is provided mainly for debugging purposes. Enabling logging will output internal library
 * information that can be used to find bugs with the help of Phidgetd Inc. Alternatively, the user
 * can enable and write to the log for their own uses.
 * @{
 */
typedef enum {
 PHIDGET_LOG_CRITICAL = 1, /**< Really important errors that can't be recovered. */
 PHIDGET_LOG_ERROR, /**< Errors that are recovered from. */
 PHIDGET_LOG_WARNING, /**< Warning's about weird things that aren't neccesarily wrong. */
 PHIDGET_LOG_DEBUG, /**< Should only be used during development - only shows up in the debug library. */
 PHIDGET_LOG_INFO, /**< Info about the going on's in the library. */
 PHIDGET_LOG_VERBOSE /**< Everything, including very common messages. */
} CPhidgetLog_level;
/**
 * Enables logging.
 * @param level The highest level of logging to output. All lower levels will also be output.
 * @param outputFile File to output log to. This should be a full pathname, not a relative pathname. Specify NULL to output to stdout.
 */
 int CPhidget_enableLogging(CPhidgetLog_level level, const char *outputFile);
/**
 * Disables logging.
 */
 int CPhidget_disableLogging();
/**
 * Appends a message to the log.
 * @param level The level at which to log the message.
 * @param id An arbitrary identifier.
 * @param message The message (printf style).
 */
 int CPhidget_log(CPhidgetLog_level level, const char *id, const char *message, ...);
/** @} */
/** \defgroup phidaccel Phidget Accelerometer 
 * \ingroup phidgets
 * Calls specific to the Phidget Accelerometer. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetAccelerometer *CPhidgetAccelerometerHandle;
 int CPhidgetAccelerometer_create(CPhidgetAccelerometerHandle *phid);
/**
 * Gets the number of acceleration axes supported by this accelerometer.
 * @param phid An attached phidget accelerometer handle.
 * @param count The axis count.
 */
 int CPhidgetAccelerometer_getAxisCount(CPhidgetAccelerometerHandle phid, int *count);
/**
 * Gets the current acceleration of an axis.
 * @param phid An attached phidget accelerometer handle.
 * @param index The acceleration index.
 * @param acceleration The acceleration.
 */
 int CPhidgetAccelerometer_getAcceleration(CPhidgetAccelerometerHandle phid, int index, double *acceleration);
/**
 * Gets the maximum accleration supported by an axis.
 * @param phid An attached phidget accelerometer handle.
 * @param index The acceleration index
 * @param max The maximum acceleration
 */
 int CPhidgetAccelerometer_getAccelerationMax(CPhidgetAccelerometerHandle phid, int index, double *max);
/**
 * Gets the minimum acceleraiton supported by an axis.
 * @param phid An attached phidget accelerometer handle.
 * @param index The acceleration index
 * @param min The minimum acceleraion
 */
 int CPhidgetAccelerometer_getAccelerationMin(CPhidgetAccelerometerHandle phid, int index, double *min);
/**
 * Sets an acceleration change event handler. This is called when the acceleration changes by more then the change trigger.
 * @param phid A phidget accelerometer handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetAccelerometer_set_OnAccelerationChange_Handler(CPhidgetAccelerometerHandle phid, int ( *fptr)(CPhidgetAccelerometerHandle phid, void *userPtr, int index, double acceleration), void *userPtr);
/**
 * Gets the change trigger for an axis.
 * @param phid An attached phidget accelerometer handle.
 * @param index The acceleration index
 * @param trigger The change trigger.
 */
 int CPhidgetAccelerometer_getAccelerationChangeTrigger(CPhidgetAccelerometerHandle phid, int index, double *trigger);
/**
 * Sets the change trigger for an axis.
 * @param phid An attached phidget accelerometer handle.
 * @param index The acceleration index
 * @param trigger The change trigger.
 */
 int CPhidgetAccelerometer_setAccelerationChangeTrigger(CPhidgetAccelerometerHandle phid, int index, double trigger);
 int CPhidgetAccelerometer_getNumAxis(CPhidgetAccelerometerHandle,int *) __attribute__ ((deprecated)) ;
/** @} */
/** \defgroup phidadvservo Phidget Advanced Servo 
 * \ingroup phidgets
 * Calls specific to the Phidget Advanced Servo. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetAdvancedServo *CPhidgetAdvancedServoHandle;
 int CPhidgetAdvancedServo_create(CPhidgetAdvancedServoHandle *phid);
/**
 * Gets the number of motors supported by this controller
 * @param phid An attached phidget advanced servo handle.
 * @param count The motor count.
 */
 int CPhidgetAdvancedServo_getMotorCount(CPhidgetAdvancedServoHandle phid, int *count);
/**
 * Gets the last set acceleration for a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param acceleration The acceleration
 */
 int CPhidgetAdvancedServo_getAcceleration(CPhidgetAdvancedServoHandle phid, int index, double *acceleration);
/**
 * Sets the acceleration for a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param acceleration The acceleration
 */
 int CPhidgetAdvancedServo_setAcceleration(CPhidgetAdvancedServoHandle phid, int index, double acceleration);
/**
 * Gets the maximum acceleration supported by a motor
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param max The maximum acceleration.
 */
 int CPhidgetAdvancedServo_getAccelerationMax(CPhidgetAdvancedServoHandle phid, int index, double *max);
/**
 * Gets the minimum acceleration supported by a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param min The minimum acceleration
 */
 int CPhidgetAdvancedServo_getAccelerationMin(CPhidgetAdvancedServoHandle phid, int index, double *min);
//set velocity
/**
 * Gets the last set velocity limit for a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param limit The velocity limit.
 */
 int CPhidgetAdvancedServo_getVelocityLimit(CPhidgetAdvancedServoHandle phid, int index, double *limit);
/**
 * Sets the velocity limit for a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param limit The velocity limit.
 */
 int CPhidgetAdvancedServo_setVelocityLimit(CPhidgetAdvancedServoHandle phid, int index, double limit);
//Actual velocity
/**
 * Gets the current velocity of a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param velocity The current velocity.
 */
 int CPhidgetAdvancedServo_getVelocity(CPhidgetAdvancedServoHandle phid, int index, double *velocity);
/**
 * Gets the maximum velocity that can be set for a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param max The maximum velocity
 */
 int CPhidgetAdvancedServo_getVelocityMax(CPhidgetAdvancedServoHandle phid, int index, double *max);
/**
 * Gets the minimum velocity that can be set for a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param min The minimum velocity.
 */
 int CPhidgetAdvancedServo_getVelocityMin(CPhidgetAdvancedServoHandle phid, int index, double *min);
/**
 * Sets a velocity change event handler. This is called when the velocity changes.
 * @param phid An attached phidget advanced servo handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetAdvancedServo_set_OnVelocityChange_Handler(CPhidgetAdvancedServoHandle phid, int ( *fptr)(CPhidgetAdvancedServoHandle phid, void *userPtr, int index, double velocity), void *userPtr);
/**
 * Gets the current position of a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param position The position.
 */
 int CPhidgetAdvancedServo_getPosition(CPhidgetAdvancedServoHandle phid, int index, double *position);
/**
 * Sets the position of a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param position The position.
 */
 int CPhidgetAdvancedServo_setPosition(CPhidgetAdvancedServoHandle phid, int index, double position);
/**
 * Gets the maximum position that a motor can go to.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param max The maximum position.
 */
 int CPhidgetAdvancedServo_getPositionMax(CPhidgetAdvancedServoHandle phid, int index, double *max);
/**
 * Sets the maximum position that a motor can go to.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param max The Maximum position.
 */
 int CPhidgetAdvancedServo_setPositionMax(CPhidgetAdvancedServoHandle phid, int index, double max);
/**
 * Gets the minimum position that a motor can go to.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param min The minimum position
 */
 int CPhidgetAdvancedServo_getPositionMin(CPhidgetAdvancedServoHandle phid, int index, double *min);
/**
 * Sets the minimum position that a motor can go to.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param min The minimum position
 */
 int CPhidgetAdvancedServo_setPositionMin(CPhidgetAdvancedServoHandle phid, int index, double min);
/**
 * Sets a position change event handler. This is called when the position changes.
 * @param phid An attached phidget advanced servo handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetAdvancedServo_set_OnPositionChange_Handler(CPhidgetAdvancedServoHandle phid, int ( *fptr)(CPhidgetAdvancedServoHandle phid, void *userPtr, int index, double position), void *userPtr);
/**
 * Gets the current current draw for a motor.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param current The current.
 */
 int CPhidgetAdvancedServo_getCurrent(CPhidgetAdvancedServoHandle phid, int index, double *current);
/**
 * Sets a current change event handler. This is called when the current draw changes.
 * @param phid An attached phidget advanced servo handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetAdvancedServo_set_OnCurrentChange_Handler(CPhidgetAdvancedServoHandle phid, int ( *fptr)(CPhidgetAdvancedServoHandle phid, void *userPtr, int index, double current), void *userPtr);
/**
 * Gets the speed ramping state for a motor. This is whether or not velocity and acceleration are used.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param rampingState The speed ramping state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetAdvancedServo_getSpeedRampingOn(CPhidgetAdvancedServoHandle phid, int index, int *rampingState);
/**
 * Sets the speed ramping state for a motor. This is whether or not velocity and acceleration are used.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param rampingState The speed ramping state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetAdvancedServo_setSpeedRampingOn(CPhidgetAdvancedServoHandle phid, int index, int rampingState);
/**
 * Gets the engaged state of a motor. This is whether the motor is powered or not.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param engagedState The engaged state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetAdvancedServo_getEngaged(CPhidgetAdvancedServoHandle phid, int index, int *engagedState);
/**
 * Sets the engaged state of a motor. This is whether the motor is powered or not.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param engagedState The engaged state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetAdvancedServo_setEngaged(CPhidgetAdvancedServoHandle phid, int index, int engagedState);
/**
 * Gets the stopped state of a motor. This is true when the motor is not moving and there are no outstanding commands.
 * @param phid An attached phidget advanced servo handle
 * @param index The motor index.
 * @param stoppedState The stopped state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetAdvancedServo_getStopped(CPhidgetAdvancedServoHandle phid, int index, int *stoppedState);
/** @} */
/** \defgroup phidenc Phidget Encoder 
 * \ingroup phidgets
 * Calls specific to the Phidget Encoder. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetEncoder *CPhidgetEncoderHandle;
 int CPhidgetEncoder_create(CPhidgetEncoderHandle *phid);
/**
 * Gets the number of digital inputs supported by this board.
 * @param phid An attached phidget encoder handle
 * @param count The input count.
 */
 int CPhidgetEncoder_getInputCount(CPhidgetEncoderHandle phid, int *count);
/**
 * Gets the state of a digital input.
 * @param phid An attached phidget encoder handle
 * @param index The input index.
 * @param inputState The input state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetEncoder_getInputState(CPhidgetEncoderHandle phid, int index, int *inputState);
/**
 * Sets an input change handler. This is called when a digital input changes.
 * @param phid An attached phidget encoder handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetEncoder_set_OnInputChange_Handler(CPhidgetEncoderHandle phid, int ( *fptr)(CPhidgetEncoderHandle phid, void *userPtr, int index, int inputState), void *userPtr);
/**
 * Gets the number of encoder inputs supported by this board.
 * @param phid An attached phidget encoder handle
 * @param count The encoder input count.
 */
 int CPhidgetEncoder_getEncoderCount(CPhidgetEncoderHandle phid, int *count);
/**
 * Gets the current position of an encoder.
 * @param phid An attached phidget encoder handle
 * @param index The encoder input index.
 * @param position The current position
 */
 int CPhidgetEncoder_getPosition(CPhidgetEncoderHandle phid, int index, int *position);
/**
 * Sets the current position of an encoder.
 * @param phid An attached phidget encoder handle
 * @param index The encoder input index.
 * @param position The new position
 */
 int CPhidgetEncoder_setPosition(CPhidgetEncoderHandle phid, int index, int position);
/**
 * Sets an encoder change handler. This is called when an encoder position changes.
 * @param phid An attached phidget encoder handle
 * @param fptr Callback function pointer. Note that positionChange is a relative not absolute change and time is the time
 *	in ms since the last position change event.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetEncoder_set_OnPositionChange_Handler(CPhidgetEncoderHandle phid, int ( *fptr)(CPhidgetEncoderHandle phid, void *userPtr, int index, int time,int positionChange), void *userPtr);
 int CPhidgetEncoder_getEncoderPosition(CPhidgetEncoderHandle, int index, int *) __attribute__ ((deprecated)) ;
 int CPhidgetEncoder_setEncoderPosition(CPhidgetEncoderHandle, int index, int) __attribute__ ((deprecated)) ;
 int CPhidgetEncoder_getNumInputs(CPhidgetEncoderHandle,int *) __attribute__ ((deprecated)) ;
 int CPhidgetEncoder_getNumEncoders(CPhidgetEncoderHandle,int *) __attribute__ ((deprecated)) ;
/** @} */
/** \defgroup phidifkit Phidget InterfaceKit 
 * \ingroup phidgets
 * Calls specific to the Phidget InterfaceKit. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetInterfaceKit *CPhidgetInterfaceKitHandle;
 int CPhidgetInterfaceKit_create(CPhidgetInterfaceKitHandle *phid);
/**
 * Gets the number of digital inputs supported by this board.
 * @param phid An attached phidget interface kit handle.
 * @param count The ditial input count.
 */
 int CPhidgetInterfaceKit_getInputCount(CPhidgetInterfaceKitHandle phid, int *count);
/**
 * Gets the state of a digital input.
 * @param phid An attached phidget interface kit handle.
 * @param index The input index.
 * @param inputState The input state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetInterfaceKit_getInputState(CPhidgetInterfaceKitHandle phid, int index, int *inputState);
/**
 * Set a digital input change handler. This is called when a digital input changes.
 * @param phid An attached phidget interface kit handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetInterfaceKit_set_OnInputChange_Handler(CPhidgetInterfaceKitHandle phid, int ( *fptr)(CPhidgetInterfaceKitHandle phid, void *userPtr, int index, int inputState), void *userPtr);
/**
 * Gets the number of digital outputs supported by this board.
 * @param phid An attached phidget interface kit handle.
 * @param count The output count.
 */
 int CPhidgetInterfaceKit_getOutputCount(CPhidgetInterfaceKitHandle phid, int *count);
/**
 * Gets the state of a digital output.
 * @param phid An attached phidget interface kit handle.
 * @param index The output index.
 * @param outputState The output state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetInterfaceKit_getOutputState(CPhidgetInterfaceKitHandle phid, int index, int *outputState);
/**
 * Sets the state of a digital output.
 * @param phid An attached phidget interface kit handle.
 * @param index The otuput index.
 * @param outputState The output state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetInterfaceKit_setOutputState(CPhidgetInterfaceKitHandle phid, int index, int outputState);
/**
 * Set a digital output change handler. This is called when a digital output changes.
 * @param phid An attached phidget interface kit handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetInterfaceKit_set_OnOutputChange_Handler(CPhidgetInterfaceKitHandle phid, int ( *fptr)(CPhidgetInterfaceKitHandle phid, void *userPtr, int index, int outputState), void *userPtr);
/**
 * Gets the number of sensor (analog) inputs supported by this board.
 * @param phid An attached phidget interface kit handle.
 * @param count The sensor input count.
 */
 int CPhidgetInterfaceKit_getSensorCount(CPhidgetInterfaceKitHandle phid, int *count);
/**
 * Gets a sensor value (0-1000).
 * @param phid An attached phidget interface kit handle.
 * @param index The sensor index.
 * @param sensorValue The sensor value.
 */
 int CPhidgetInterfaceKit_getSensorValue(CPhidgetInterfaceKitHandle phid, int index, int *sensorValue);
/**
 * Gets a sensor raw value (12-bit).
 * @param phid An attached phidget interface kit handle.
 * @param index The sensor index.
 * @param sensorRawValue The sensor value.
 */
 int CPhidgetInterfaceKit_getSensorRawValue(CPhidgetInterfaceKitHandle phid, int index, int *sensorRawValue);
/**
 * Set a sensor change handler. This is called when a sensor value changes by more then the change trigger.
 * @param phid An attached phidget interface kit handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetInterfaceKit_set_OnSensorChange_Handler(CPhidgetInterfaceKitHandle phid, int ( *fptr)(CPhidgetInterfaceKitHandle phid, void *userPtr, int index, int sensorValue), void *userPtr);
/**
 * Gets a sensor change trigger.
 * @param phid An attached phidget interface kit handle.
 * @param index The sensor index.
 * @param trigger The change trigger.
 */
 int CPhidgetInterfaceKit_getSensorChangeTrigger(CPhidgetInterfaceKitHandle phid, int index, int *trigger);
/**
 * Sets a sensor change trigger.
 * @param phid An attached phidget interface kit handle.
 * @param index The sensor index.
 * @param trigger The change trigger.
 */
 int CPhidgetInterfaceKit_setSensorChangeTrigger(CPhidgetInterfaceKitHandle phid, int index, int trigger);
/**
 * Gets the ratiometric state for this board.
 * @param phid An attached phidget interface kit handle.
 * @param ratiometric The ratiometric state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetInterfaceKit_getRatiometric(CPhidgetInterfaceKitHandle phid, int *ratiometric);
/**
 * Sets the ratiometric state for this board.
 * @param phid An attached phidget interface kit handle.
 * @param ratiometric The ratiometric state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetInterfaceKit_setRatiometric(CPhidgetInterfaceKitHandle phid, int ratiometric);
 int CPhidgetInterfaceKit_getNumInputs(CPhidgetInterfaceKitHandle,int *) __attribute__ ((deprecated)) ;
 int CPhidgetInterfaceKit_getNumOutputs(CPhidgetInterfaceKitHandle,int *) __attribute__ ((deprecated)) ;
 int CPhidgetInterfaceKit_getNumSensors(CPhidgetInterfaceKitHandle,int *) __attribute__ ((deprecated)) ;
/** @} */
/** \defgroup phidled Phidget LED 
 * \ingroup phidgets
 * Calls specific to the Phidget LED. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetLED *CPhidgetLEDHandle;
 int CPhidgetLED_create(CPhidgetLEDHandle *phid);
/**
 * Gets the number of LEDs supported by this board.
 * @param phid An attached phidget LED handle.
 * @param count The led count.
 */
 int CPhidgetLED_getLEDCount(CPhidgetLEDHandle phid, int *count);
/**
 * Gets the brightness of an LED.
 * @param phid An attached phidget LED handle.
 * @param index The LED index.
 * @param brightness The LED brightness (0-100).
 */
 int CPhidgetLED_getDiscreteLED(CPhidgetLEDHandle phid, int index, int *brightness);
/**
 * Sets the brightness of an LED.
 * @param phid An attached phidget LED handle.
 * @param index The LED index.
 * @param brightness The LED brightness (0-100).
 */
 int CPhidgetLED_setDiscreteLED(CPhidgetLEDHandle phid, int index, int brightness);
 int CPhidgetLED_getNumLEDs(CPhidgetLEDHandle,int *) __attribute__ ((deprecated)) ;
/** @} */
/** \defgroup phidmotorcontrol Phidget Motor Control 
 * \ingroup phidgets
 * Calls specific to the Phidget Motor Control. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetMotorControl *CPhidgetMotorControlHandle;
 int CPhidgetMotorControl_create(CPhidgetMotorControlHandle *phid);
/**
 * Gets the number of motors supported by this controller.
 * @param phid An attached phidget motor control handle.
 * @param count The motor count.
 */
 int CPhidgetMotorControl_getMotorCount(CPhidgetMotorControlHandle phid, int *count);
/**
 * Gets the current velocity of a motor.
 * @param phid An attached phidget motor control handle.
 * @param index The motor index.
 * @param velocity The current velocity.
 */
 int CPhidgetMotorControl_getVelocity(CPhidgetMotorControlHandle phid, int index, double *velocity);
/**
 * Sets the velocity of a motor.
 * @param phid An attached phidget motor control handle.
 * @param index The motor index.
 * @param velocity The velocity.
 */
 int CPhidgetMotorControl_setVelocity(CPhidgetMotorControlHandle phid, int index, double velocity);
/**
 * Sets a velocity change event handler. This is called when the velocity changes.
 * @param phid An attached phidget motor control handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetMotorControl_set_OnVelocityChange_Handler(CPhidgetMotorControlHandle phid, int ( *fptr)(CPhidgetMotorControlHandle phid, void *userPtr, int index, double velocity), void *userPtr);
/**
 * Gets the last set acceleration of a motor.
 * @param phid An attached phidget motor control handle.
 * @param index The motor index.
 * @param acceleration The acceleration.
 */
 int CPhidgetMotorControl_getAcceleration(CPhidgetMotorControlHandle phid, int index, double *acceleration);
/**
 * Sets the last set acceleration of a motor.
 * @param phid An attached phidget motor control handle.
 * @param index The motor index.
 * @param acceleration The acceleration.
 */
 int CPhidgetMotorControl_setAcceleration(CPhidgetMotorControlHandle phid, int index, double acceleration);
/**
 * Gets the maximum acceleration supported by a motor
 * @param phid An attached phidget motor control handle
 * @param index The motor index.
 * @param max The maximum acceleration.
 */
 int CPhidgetMotorControl_getAccelerationMax(CPhidgetMotorControlHandle phid, int index, double *max);
/**
 * Gets the minimum acceleration supported by a motor.
 * @param phid An attached phidget motor control handle
 * @param index The motor index.
 * @param min The minimum acceleration
 */
 int CPhidgetMotorControl_getAccelerationMin(CPhidgetMotorControlHandle phid, int index, double *min);
/**
 * Gets the current current draw for a motor.
 * @param phid An attached phidget motor control handle
 * @param index The motor index.
 * @param current The current.
 */
 int CPhidgetMotorControl_getCurrent(CPhidgetMotorControlHandle phid, int index, double *current);
/**
 * Sets a current change event handler. This is called when the current draw changes.
 * @param phid An attached phidget motor control handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetMotorControl_set_OnCurrentChange_Handler(CPhidgetMotorControlHandle phid, int ( *fptr)(CPhidgetMotorControlHandle phid, void *userPtr, int index, double current), void *userPtr);
/**
 * Gets the number of digital inputs supported by this board.
 * @param phid An attached phidget motor control handle.
 * @param count The ditial input count.
 */
 int CPhidgetMotorControl_getInputCount(CPhidgetMotorControlHandle phid, int *count);
/**
 * Gets the state of a digital input.
 * @param phid An attached phidget motor control handle.
 * @param index The input index.
 * @param inputState The input state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetMotorControl_getInputState(CPhidgetMotorControlHandle phid, int index, int *inputState);
/**
 * Set a digital input change handler. This is called when a digital input changes.
 * @param phid An attached phidget motor control handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetMotorControl_set_OnInputChange_Handler(CPhidgetMotorControlHandle phid, int ( *fptr)(CPhidgetMotorControlHandle phid, void *userPtr, int index, int inputState), void *userPtr);
 int CPhidgetMotorControl_getNumMotors(CPhidgetMotorControlHandle,int *) __attribute__ ((deprecated)) ;
 int CPhidgetMotorControl_getNumInputs(CPhidgetMotorControlHandle,int *) __attribute__ ((deprecated)) ;
 int CPhidgetMotorControl_getMotorSpeed(CPhidgetMotorControlHandle, int index, double *) __attribute__ ((deprecated)) ;
 int CPhidgetMotorControl_setMotorSpeed(CPhidgetMotorControlHandle, int index, double) __attribute__ ((deprecated)) ;
 int CPhidgetMotorControl_set_OnMotorChange_Handler(CPhidgetMotorControlHandle, int ( *fptr)(CPhidgetMotorControlHandle, void *userPtr, int index, double motorSpeed), void *userPtr) __attribute__ ((deprecated)) ;
/** @} */
/** \defgroup phidph Phidget PH Sensor 
 * \ingroup phidgets
 * Calls specific to the Phidget PH Sensor. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetPHSensor *CPhidgetPHSensorHandle;
 int CPhidgetPHSensor_create(CPhidgetPHSensorHandle *phid);
/**
 * Gets the sensed PH.
 * @param phid An attached phidget ph sensor handle.
 * @param ph The PH.
 */
 int CPhidgetPHSensor_getPH(CPhidgetPHSensorHandle phid, double *ph);
/**
 * Gets the maximum PH that the sensor could report.
 * @param phid An attached phidget ph sensor handle.
 * @param max The maximum PH.
 */
 int CPhidgetPHSensor_getPHMax(CPhidgetPHSensorHandle phid, double *max);
/**
 * Gets the minimum PH that the sensor could report.
 * @param phid An attached phidget ph sensor handle.
 * @param min The minimum PH.
 */
 int CPhidgetPHSensor_getPHMin(CPhidgetPHSensorHandle phid, double *min);
/**
 * Set a PH change handler. This is called when the PH changes by more then the change trigger.
 * @param phid An attached phidget ph sensor handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetPHSensor_set_OnPHChange_Handler(CPhidgetPHSensorHandle phid, int ( *fptr)(CPhidgetPHSensorHandle phid, void *userPtr, double ph), void *userPtr);
/**
 * Gets the PH change trigger.
 * @param phid An attached phidget ph sensor handle.
 * @param trigger The change trigger.
 */
 int CPhidgetPHSensor_getPHChangeTrigger(CPhidgetPHSensorHandle phid, double *trigger);
/**
 * Sets the PH change trigger.
 * @param phid An attached phidget ph sensor handle.
 * @param trigger The change trigger.
 */
 int CPhidgetPHSensor_setPHChangeTrigger(CPhidgetPHSensorHandle phid, double trigger);
/**
 * Gets the sensed potential.
 * @param phid An attached phidget ph sensor handle.
 * @param potential The potential.
 */
 int CPhidgetPHSensor_getPotential(CPhidgetPHSensorHandle phid, double *potential);
/**
 * Gets the maximum potential that can be sensed.
 * @param phid An attached phidget ph sensor handle.
 * @param max The maximum potential.
 */
 int CPhidgetPHSensor_getPotentialMax(CPhidgetPHSensorHandle phid, double *max);
/**
 * Gets the minimum potential that can be sensed.
 * @param phid An attached phidget ph sensor handle.
 * @param min The minimum potential.
 */
 int CPhidgetPHSensor_getPotentialMin(CPhidgetPHSensorHandle phid, double *min);
/**
 * Sets the temperature to be used for PH calculations.
 * @param phid An attached phidget ph sensor handle.
 * @param temperature The temperature (degrees celcius). By default this is 20.
 */
 int CPhidgetPHSensor_setTemperature(CPhidgetPHSensorHandle phid, double temperature);
/** @} */
/** \defgroup phidrfid Phidget RFID 
 * \ingroup phidgets
 * Calls specific to the Phidget RFID. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetRFID *CPhidgetRFIDHandle;
 int CPhidgetRFID_create(CPhidgetRFIDHandle *phid);
/**
 * Gets the number of outputs supported by this board.
 * @param phid An attached phidget rfid handle.
 * @param count The output count.
 */
 int CPhidgetRFID_getOutputCount(CPhidgetRFIDHandle phid, int *count);
/**
 * Gets the state of an output.
 * @param phid An attached phidget rfid handle.
 * @param index The output index.
 * @param outputState The output state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetRFID_getOutputState(CPhidgetRFIDHandle phid, int index, int *outputState);
/**
 * Sets the state of an output.
 * @param phid An attached phidget rfid handle.
 * @param index The output index.
 * @param outputState The output state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetRFID_setOutputState(CPhidgetRFIDHandle phid, int index, int outputState);
/**
 * Set an output change handler. This is called when an output changes.
 * @param phid An attached phidget rfid handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetRFID_set_OnOutputChange_Handler(CPhidgetRFIDHandle phid, int ( *fptr)(CPhidgetRFIDHandle phid, void *userPtr, int index, int outputState), void *userPtr);
/**
 * Gets the state of the antenna.
 * @param phid An attached phidget rfid handle.
 * @param antennaState The antenna state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetRFID_getAntennaOn(CPhidgetRFIDHandle phid, int *antennaState);
/**
 * Sets the state of the antenna. Note that the antenna must be enabled before tags will be read.
 * @param phid An attached phidget rfid handle.
 * @param antennaState The antenna state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetRFID_setAntennaOn(CPhidgetRFIDHandle phid, int antennaState);
/**
 * Gets the state of the onboard LED.
 * @param phid An attached phidget rfid handle.
 * @param LEDState The LED state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetRFID_getLEDOn(CPhidgetRFIDHandle phid, int *LEDState);
/**
 * Sets the state of the onboard LED.
 * @param phid An attached phidget rfid handle.
 * @param LEDState The LED state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetRFID_setLEDOn(CPhidgetRFIDHandle phid, int LEDState);
/**
 * Gets the last tag read by the reader. This tag may or may not still be on the reader.
 * @param phid An attached phidget rfid handle.
 * @param tag The tag.
 */
 int CPhidgetRFID_getLastTag(CPhidgetRFIDHandle phid, unsigned char *tag);
/**
 * Gets the tag present status. This is whether or not a tag is being read by the reader.
 * @param phid An attached phidget rfid handle.
 * @param status The tag status. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetRFID_getTagStatus(CPhidgetRFIDHandle phid, int *status);
/**
 * Set a tag handler. This is called when a tag is first detected by the reader.
 * @param phid An attached phidget rfid handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetRFID_set_OnTag_Handler(CPhidgetRFIDHandle phid, int ( *fptr)(CPhidgetRFIDHandle phid, void *userPtr, unsigned char *tag), void *userPtr);
/**
 * Set a tag lost handler. This is called when a tag is no longer detected by the reader.
 * @param phid An attached phidget rfid handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetRFID_set_OnTagLost_Handler(CPhidgetRFIDHandle phid, int ( *fptr)(CPhidgetRFIDHandle phid, void *userPtr, unsigned char *tag), void *userPtr);
 int CPhidgetRFID_getNumOutputs(CPhidgetRFIDHandle,int *) __attribute__ ((deprecated)) ;
/** @} */
/** \defgroup phidservo Phidget Servo 
 * \ingroup phidgets
 * Calls specific to the Phidget Servo. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetServo *CPhidgetServoHandle;
 int CPhidgetServo_create(CPhidgetServoHandle *phid);
/**
 * Gets the number of motors supported by this controller.
 * @param phid An attached phidget servo handle.
 * @param count The motor count.
 */
 int CPhidgetServo_getMotorCount(CPhidgetServoHandle phid, int *count);
/**
 * Gets the current position of a motor.
 * @param phid An attached phidget servo handle.
 * @param index The motor index.
 * @param position The motor position.
 */
 int CPhidgetServo_getPosition(CPhidgetServoHandle phid, int index, double *position);
/**
 * Sets the current position of a motor.
 * @param phid An attached phidget servo handle.
 * @param index The motor index.
 * @param position The motor position.
 */
 int CPhidgetServo_setPosition(CPhidgetServoHandle phid, int index, double position);
/**
 * Gets the maximum position that a motor can be set to.
 * @param phid An attached phidget servo handle.
 * @param index The motor index.
 * @param max The maximum position.
 */
 int CPhidgetServo_getPositionMax(CPhidgetServoHandle phid, int index, double *max);
/**
 * Gets the minimum position that a motor can be set to.
 * @param phid An attached phidget servo handle.
 * @param index The motor index.
 * @param min The minimum position.
 */
 int CPhidgetServo_getPositionMin(CPhidgetServoHandle phid, int index, double *min);
/**
 * Sets a position change event handler. This is called when the position changes.
 * @param phid An attached phidget servo handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetServo_set_OnPositionChange_Handler(CPhidgetServoHandle phid, int ( *fptr)(CPhidgetServoHandle phid, void *userPtr, int index, double position), void *userPtr);
/**
 * Gets the engaged state of a motor. This is whether the motor is powered or not.
 * @param phid An attached phidget servo handle
 * @param index The motor index.
 * @param engagedState The engaged state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetServo_getEngaged(CPhidgetServoHandle phid, int index, int *engagedState);
/**
 * Sets the engaged state of a motor. This is whether the motor is powered or not.
 * @param phid An attached phidget servo handle
 * @param index The motor index.
 * @param engagedState The engaged state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetServo_setEngaged(CPhidgetServoHandle phid, int index, int engagedState);
 int CPhidgetServo_getNumMotors(CPhidgetServoHandle,int *) __attribute__ ((deprecated)) ;
 int CPhidgetServo_setMotorPosition(CPhidgetServoHandle, int index, double) __attribute__ ((deprecated)) ;
 int CPhidgetServo_getMotorPosition(CPhidgetServoHandle, int index, double *) __attribute__ ((deprecated)) ;
 int CPhidgetServo_getMotorPositionMax(CPhidgetServoHandle, int index, double *) __attribute__ ((deprecated)) ;
 int CPhidgetServo_getMotorPositionMin(CPhidgetServoHandle, int index, double *) __attribute__ ((deprecated)) ;
 int CPhidgetServo_set_OnMotorPositionChange_Handler(CPhidgetServoHandle, int ( *fptr)(CPhidgetServoHandle, void *userPtr, int index, double motorPosition), void *userPtr) __attribute__ ((deprecated)) ;
 int CPhidgetServo_setMotorOn(CPhidgetServoHandle, int index, int) __attribute__ ((deprecated)) ;
 int CPhidgetServo_getMotorOn(CPhidgetServoHandle, int index, int *) __attribute__ ((deprecated)) ;
/** @} */
/** \defgroup phidstepper Phidget Stepper 
 * \ingroup phidgets
 * Calls specific to the Phidget Stepper. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetStepper *CPhidgetStepperHandle;
 int CPhidgetStepper_create(CPhidgetStepperHandle *phid);
/**
 * Gets the number of digital inputs supported by this board.
 * @param phid An attached phidget stepper handle.
 * @param count The ditial input count.
 */
 int CPhidgetStepper_getInputCount(CPhidgetStepperHandle phid, int *count);
/**
 * Gets the state of a digital input.
 * @param phid An attached phidget stepper handle.
 * @param index The input index.
 * @param inputState The input state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetStepper_getInputState(CPhidgetStepperHandle phid, int index, int *inputState);
/**
 * Set a digital input change handler. This is called when a digital input changes.
 * @param phid An attached phidget stepper handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetStepper_set_OnInputChange_Handler(CPhidgetStepperHandle phid, int ( *fptr)(CPhidgetStepperHandle phid, void *userPtr, int index, int inputState), void *userPtr);
/**
 * Gets the number of motors supported by this controller
 * @param phid An attached phidget stepper handle.
 * @param count The motor count.
 */
 int CPhidgetStepper_getMotorCount(CPhidgetStepperHandle phid, int *count);
/**
 * Gets the last set acceleration for a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param acceleration The acceleration
 */
 int CPhidgetStepper_getAcceleration(CPhidgetStepperHandle phid, int index, double *acceleration);
/**
 * Sets the acceleration for a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param acceleration The acceleration
 */
 int CPhidgetStepper_setAcceleration(CPhidgetStepperHandle phid, int index, double acceleration);
/**
 * Gets the maximum acceleration supported by a motor
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param max The maximum acceleration.
 */
 int CPhidgetStepper_getAccelerationMax(CPhidgetStepperHandle phid, int index, double *max);
/**
 * Gets the minimum acceleration supported by a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param min The minimum acceleration
 */
 int CPhidgetStepper_getAccelerationMin(CPhidgetStepperHandle phid, int index, double *min);
/**
 * Gets the last set velocity limit for a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param limit The velocity limit.
 */
 int CPhidgetStepper_getVelocityLimit(CPhidgetStepperHandle phid, int index, double *limit);
/**
 * Sets the velocity limit for a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param limit The velocity limit.
 */
 int CPhidgetStepper_setVelocityLimit(CPhidgetStepperHandle phid, int index, double limit);
/**
 * Gets the current velocity of a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param velocity The current velocity.
 */
 int CPhidgetStepper_getVelocity(CPhidgetStepperHandle phid, int index, double *velocity);
/**
 * Gets the maximum velocity that can be set for a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param max The maximum velocity
 */
 int CPhidgetStepper_getVelocityMax(CPhidgetStepperHandle phid, int index, double *max);
/**
 * Gets the minimum velocity that can be set for a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param min The minimum velocity.
 */
 int CPhidgetStepper_getVelocityMin(CPhidgetStepperHandle phid, int index, double *min);
/**
 * Sets a velocity change event handler. This is called when the velocity changes.
 * @param phid An attached phidget stepper handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetStepper_set_OnVelocityChange_Handler(CPhidgetStepperHandle phid, int ( *fptr)(CPhidgetStepperHandle phid, void *userPtr, int index, double velocity), void *userPtr);
/**
 * Gets the last set target position of a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param position The position.
 */
 int CPhidgetStepper_getTargetPosition(CPhidgetStepperHandle phid, int index, __int64 *position);
/**
 * Sets the target position of a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param position The position.
 */
 int CPhidgetStepper_setTargetPosition(CPhidgetStepperHandle phid, int index, __int64 position);
/**
 * Gets the current position of a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param position The position.
 */
 int CPhidgetStepper_getCurrentPosition(CPhidgetStepperHandle phid, int index, __int64 *position);
/**
 * Sets the current position of a motor. This will not move the motor, just update the position value.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param position The position.
 */
 int CPhidgetStepper_setCurrentPosition(CPhidgetStepperHandle phid, int index, __int64 position);
/**
 * Gets the maximum position that a motor can go to.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param max The maximum position.
 */
 int CPhidgetStepper_getPositionMax(CPhidgetStepperHandle phid, int index, __int64 *max);
/**
 * Gets the minimum position that a motor can go to.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param min The minimum position
 */
 int CPhidgetStepper_getPositionMin(CPhidgetStepperHandle phid, int index, __int64 *min);
/**
 * Sets a position change event handler. This is called when the position changes.
 * @param phid An attached phidget stepper handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetStepper_set_OnPositionChange_Handler(CPhidgetStepperHandle phid, int ( *fptr)(CPhidgetStepperHandle phid, void *userPtr, int index, __int64 position), void *userPtr);
/**
 * Gets the current limit for a motor.
 * @param phid An attached phidget stepper handle.
 * @param index The motor index.
 * @param limit The current limit.
 */
 int CPhidgetStepper_getCurrentLimit(CPhidgetStepperHandle phid, int index, double *limit);
/**
 * Sets the current limit for a motor.
 * @param phid An attached phidget stepper handle.
 * @param index The motor index.
 * @param limit The current limit.
 */
 int CPhidgetStepper_setCurrentLimit(CPhidgetStepperHandle phid, int index, double limit);
/**
 * Gets the current current draw for a motor.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param current The current.
 */
 int CPhidgetStepper_getCurrent(CPhidgetStepperHandle phid, int index, double *current);
/**
 * Gets the maximum current limit.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param max The maximum current limit.
 */
 int CPhidgetStepper_getCurrentMax(CPhidgetStepperHandle phid, int index, double *max);
/**
 * Gets the minimum current limit.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param min The minimum current limit.
 */
 int CPhidgetStepper_getCurrentMin(CPhidgetStepperHandle phid, int index, double *min);
/**
 * Sets a current change event handler. This is called when the current draw changes.
 * @param phid An attached phidget stepper handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetStepper_set_OnCurrentChange_Handler(CPhidgetStepperHandle phid, int ( *fptr)(CPhidgetStepperHandle phid, void *userPtr, int index, double current), void *userPtr);
/**
 * Gets the engaged state of a motor. This is whether the motor is powered or not.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param engagedState The engaged state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetStepper_getEngaged(CPhidgetStepperHandle phid, int index, int *engagedState);
/**
 * Sets the engaged state of a motor. This is whether the motor is powered or not.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param engagedState The engaged state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetStepper_setEngaged(CPhidgetStepperHandle phid, int index, int engagedState);
/**
 * Gets the stopped state of a motor. This is true when the motor is not moving and there are no outstanding commands.
 * @param phid An attached phidget stepper handle
 * @param index The motor index.
 * @param stoppedState The stopped state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetStepper_getStopped(CPhidgetStepperHandle phid, int index, int *stoppedState);
/** @} */
/** \defgroup phidtemp Phidget Temperature Sensor 
 * \ingroup phidgets
 * Calls specific to the Phidget Temperature Sensor. See the product manual for more specific API details, supported functionality, units, etc.
 *
 * All temperatures are in degrees celcius.
 * @{
 */
typedef struct _CPhidgetTemperatureSensor *CPhidgetTemperatureSensorHandle;
 int CPhidgetTemperatureSensor_create(CPhidgetTemperatureSensorHandle *phid);
/**
 * The Phidget Temperature Sensor supports these types of thermocouples.
 */
typedef enum {
 PHIDGET_TEMPERATURE_SENSOR_K_TYPE = 1, /**< K-Type thermocouple */
 PHIDGET_TEMPERATURE_SENSOR_J_TYPE, /**< J-Type thermocouple */
 PHIDGET_TEMPERATURE_SENSOR_E_TYPE, /**< E-Type thermocouple */
 PHIDGET_TEMPERATURE_SENSOR_T_TYPE /**< T-Type thermocouple */
} CPhidgetTemperatureSensor_ThermocoupleType;
/**
 * Gets the number of thermocouple inputs supported by this board.
 * @param phid An attached phidget themperature sensor handle.
 * @param count The thermocouple input count.
 */
 int CPhidgetTemperatureSensor_getTemperatureInputCount(CPhidgetTemperatureSensorHandle phid, int *count);
/**
 * Gets the temperature measured by a thermocouple input.
 * @param phid An attached phidget themperature sensor handle.
 * @param index The thermocouple index.
 * @param temperature The temperature.
 */
 int CPhidgetTemperatureSensor_getTemperature(CPhidgetTemperatureSensorHandle phid, int index, double *temperature);
/**
 * Gets the maximum temperature that can be measured by a thermocouple input. This depends on the type of thermocouple attached.
 * @param phid An attached phidget themperature sensor handle.
 * @param index The thermocouple index.
 * @param max The maximum temperature.
 */
 int CPhidgetTemperatureSensor_getTemperatureMax(CPhidgetTemperatureSensorHandle phid, int index, double *max);
/**
 * Gets the minimum temperature that can be measured by a thermocouple input. This depends on the type of thermocouple attached.
 * @param phid An attached phidget themperature sensor handle.
 * @param index The thermocouple index.
 * @param min The minimum temperature.
 */
 int CPhidgetTemperatureSensor_getTemperatureMin(CPhidgetTemperatureSensorHandle phid, int index, double *min);
/**
 * Set a temperature change handler. This is called when the temperature changes by more then the change trigger.
 * @param phid An attached phidget temperature sensor handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetTemperatureSensor_set_OnTemperatureChange_Handler(CPhidgetTemperatureSensorHandle phid, int ( *fptr)(CPhidgetTemperatureSensorHandle phid, void *userPtr, int index, double temperature), void *userPtr);
/**
 * Gets the change trigger for a thermocouple input.
 * @param phid An attached phidget themperature sensor handle.
 * @param index The thermocouple index.
 * @param trigger The change trigger.
 */
 int CPhidgetTemperatureSensor_getTemperatureChangeTrigger(CPhidgetTemperatureSensorHandle phid, int index, double *trigger);
/**
 * Sets the change trigger for a thermocouple input.
 * @param phid An attached phidget themperature sensor handle.
 * @param index The thermocouple index.
 * @param trigger The change trigger.
 */
 int CPhidgetTemperatureSensor_setTemperatureChangeTrigger(CPhidgetTemperatureSensorHandle phid, int index, double trigger);
/**
 * Gets the currently sensed potential for a thermocouple input.
 * @param phid An attached phidget themperature sensor handle.
 * @param index The thermocouple index.
 * @param potential The potential.
 */
 int CPhidgetTemperatureSensor_getPotential(CPhidgetTemperatureSensorHandle phid, int index, double *potential);
/**
 * Gets the maximum potential that a thermocouple input can measure.
 * @param phid An attached phidget themperature sensor handle.
 * @param index The thermocouple index.
 * @param max The maximum potential.
 */
 int CPhidgetTemperatureSensor_getPotentialMax(CPhidgetTemperatureSensorHandle phid, int index, double *max);
/**
 * Gets the minimum potential that a thermocouple input can measure.
 * @param phid An attached phidget themperature sensor handle.
 * @param index The thermocouple index.
 * @param min The minimum potential.
 */
 int CPhidgetTemperatureSensor_getPotentialMin(CPhidgetTemperatureSensorHandle phid, int index, double *min);
/**
 * Gets the ambient (board) temperature.
 * @param phid An attached phidget themperature sensor handle.
 * @param ambient The ambient (board) temperature.
 */
 int CPhidgetTemperatureSensor_getAmbientTemperature(CPhidgetTemperatureSensorHandle phid, double *ambient);
/**
 * Gets the maximum temperature that the ambient onboard temperature sensor can measure.
 * @param phid An attached phidget themperature sensor handle.
 * @param max The maximum temperature.
 */
 int CPhidgetTemperatureSensor_getAmbientTemperatureMax(CPhidgetTemperatureSensorHandle phid, double *max);
/**
 * Gets the minimum temperature that the ambient onboard temperature sensor can measure.
 * @param phid An attached phidget themperature sensor handle.
 * @param min The minimum temperature.
 */
 int CPhidgetTemperatureSensor_getAmbientTemperatureMin(CPhidgetTemperatureSensorHandle phid, double *min);
/**
 * Gets the type of thermocouple set to be at a thermocouple input. By default this is K-Type.
 * @param phid An attached phidget themperature sensor handle.
 * @param index The thermocouple index.
 * @param type The thermocouple type.
 */
 int CPhidgetTemperatureSensor_getThermocoupleType(CPhidgetTemperatureSensorHandle phid, int index, CPhidgetTemperatureSensor_ThermocoupleType *type);
/**
 * Sets the type of thermocouple plugged into a thermocouple input. By default this is K-Type.
 * @param phid An attached phidget themperature sensor handle.
 * @param index The thermocouple index.
 * @param type The thermocouple type.
 */
 int CPhidgetTemperatureSensor_setThermocoupleType(CPhidgetTemperatureSensorHandle phid, int index, CPhidgetTemperatureSensor_ThermocoupleType type);
 int CPhidgetTemperatureSensor_getNumTemperatureInputs(CPhidgetTemperatureSensorHandle,int *) __attribute__ ((deprecated)) ;
/** @} */
/** \defgroup phidtextlcd Phidget TextLCD 
 * \ingroup phidgets
 * Calls specific to the Phidget Text LCD. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetTextLCD *CPhidgetTextLCDHandle;
 int CPhidgetTextLCD_create(CPhidgetTextLCDHandle *phid);
/**
 * Gets the number of rows supported by this display.
 * @param phid An attached phidget text lcd handle.
 * @param count The row count.
 */
 int CPhidgetTextLCD_getRowCount(CPhidgetTextLCDHandle phid, int *count);
/**
 * Gets the number of columns per supported by this display.
 * @param phid An attached phidget text lcd handle.
 * @param count The Column count.
 */
 int CPhidgetTextLCD_getColumnCount(CPhidgetTextLCDHandle phid, int *count);
/**
 * Gets the state of the backlight.
 * @param phid An attached phidget text lcd handle.
 * @param backlightState The backlight state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetTextLCD_getBacklight(CPhidgetTextLCDHandle phid, int *backlightState);
/**
 * Sets the state of the backlight.
 * @param phid An attached phidget text lcd handle.
 * @param backlightState The backlight state. Possible values are \ref PTRUE and \ref PFALSE.
 */
 int CPhidgetTextLCD_setBacklight(CPhidgetTextLCDHandle phid, int backlightState);
/**
 * Gets the last set contrast value.
 * @param phid An attached phidget text lcd handle.
 * @param contrast The contrast (0-255).
 */
 int CPhidgetTextLCD_getContrast(CPhidgetTextLCDHandle phid, int *contrast);
/**
 * Sets the last set contrast value.
 * @param phid An attached phidget text lcd handle.
 * @param contrast The contrast (0-255).
 */
 int CPhidgetTextLCD_setContrast(CPhidgetTextLCDHandle phid, int contrast);
/**
 * Gets the cursor visible state.
 * @param phid An attached phidget text lcd handle.
 * @param cursorState The state of the cursor.
 */
 int CPhidgetTextLCD_getCursorOn(CPhidgetTextLCDHandle phid, int *cursorState);
/**
 * Sets the cursor visible state.
 * @param phid An attached phidget text lcd handle.
 * @param cursorState The state of the cursor.
 */
 int CPhidgetTextLCD_setCursorOn(CPhidgetTextLCDHandle phid, int cursorState);
/**
 * Gets the cursor blink state.
 * @param phid An attached phidget text lcd handle.
 * @param cursorBlinkState The cursor blink state.
 */
 int CPhidgetTextLCD_getCursorBlink(CPhidgetTextLCDHandle phid, int *cursorBlinkState);
/**
 * Sets the cursor blink state.
 * @param phid An attached phidget text lcd handle.
 * @param cursorBlinkState The cursor blink state.
 */
 int CPhidgetTextLCD_setCursorBlink(CPhidgetTextLCDHandle phid, int cursorBlinkState);
/**
 * Sets a custom character. See the product manual for more information.
 * @param phid An attached phidget text lcd handle.
 * @param index The custom character index (8-15).
 * @param var1 The first part of the custom character.
 * @param var2 The second part of the custom character.
 */
 int CPhidgetTextLCD_setCustomCharacter(CPhidgetTextLCDHandle phid, int index, int var1,int var2);
/**
 * Sets a single character on the display.
 * @param phid An attached phidget text lcd handle.
 * @param index The row index.
 * @param column The column index.
 * @param character The character to display.
 */
 int CPhidgetTextLCD_setDisplayCharacter(CPhidgetTextLCDHandle phid, int index, int column,unsigned char character);
/**
 * Sets a row on the display.
 * @param phid An attached phidget text lcd handle.
 * @param index The row index.
 * @param displayString The string to display. Make sure this is not longer then \ref CPhidgetTextLCD_getColumnCount.
 */
 int CPhidgetTextLCD_setDisplayString(CPhidgetTextLCDHandle phid, int index, char *displayString);
 int CPhidgetTextLCD_getNumRows(CPhidgetTextLCDHandle,int *) __attribute__ ((deprecated)) ;
 int CPhidgetTextLCD_getNumColumns(CPhidgetTextLCDHandle,int *) __attribute__ ((deprecated)) ;
/** @} */
/** \defgroup phidtextled Phidget TextLED 
 * \ingroup phidgets
 * Calls specific to the Phidget Text LED. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetTextLED *CPhidgetTextLEDHandle;
 int CPhidgetTextLED_create(CPhidgetTextLEDHandle *phid);
/**
 * Gets the number of rows supported by this display.
 * @param phid An attached phidget text led handle.
 * @param count The row count.
 */
 int CPhidgetTextLED_getRowCount(CPhidgetTextLEDHandle phid, int *count);
/**
 * Gets the number of columns per supported by this display.
 * @param phid An attached phidget text led handle.
 * @param count The Column count.
 */
 int CPhidgetTextLED_getColumnCount(CPhidgetTextLEDHandle phid, int *count);
/**
 * Gets the last set brightness value.
 * @param phid An attached phidget text led handle.
 * @param brightness The brightness (0-100).
 */
 int CPhidgetTextLED_getBrightness(CPhidgetTextLEDHandle phid, int *brightness);
/**
 * Sets the last set brightness value.
 * @param phid An attached phidget text led handle.
 * @param brightness The brightness (0-100).
 */
 int CPhidgetTextLED_setBrightness(CPhidgetTextLEDHandle phid, int brightness);
/**
 * Sets a row on the display.
 * @param phid An attached phidget text led handle.
 * @param index The row index.
 * @param displayString The string to display. Make sure this is not longer then \ref CPhidgetTextLED_getColumnCount.
 */
 int CPhidgetTextLED_setDisplayString(CPhidgetTextLEDHandle phid, int index, char *displayString);
 int CPhidgetTextLED_getNumRows(CPhidgetTextLEDHandle,int *) __attribute__ ((deprecated)) ;
 int CPhidgetTextLED_getNumColumns(CPhidgetTextLEDHandle,int *) __attribute__ ((deprecated)) ;
/** @} */
/** \defgroup phidweight Phidget Weight Sensor 
 * \ingroup phidgets
 * Calls specific to the Phidget Weight Sensor. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */
typedef struct _CPhidgetWeightSensor *CPhidgetWeightSensorHandle;
 int CPhidgetWeightSensor_create(CPhidgetWeightSensorHandle *phid);
/**
 * Gets the sensed weight.
 * @param phid An attached phidget weight sensor handle.
 * @param weight The weight.
 */
 int CPhidgetWeightSensor_getWeight(CPhidgetWeightSensorHandle phid, double *weight);
/**
 * Set a weight change handler. This is called when the weight changes by more then the change trigger.
 * @param phid An attached phidget weight sensor handle.
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
 int CPhidgetWeightSensor_set_OnWeightChange_Handler(CPhidgetWeightSensorHandle phid, int ( *fptr)(CPhidgetWeightSensorHandle phid, void *userPtr, double weight), void *userPtr);
/**
 * Gets the weight change trigger.
 * @param phid An attached phidget weight sensor handle.
 * @param trigger The change trigger.
 */
 int CPhidgetWeightSensor_getWeightChangeTrigger(CPhidgetWeightSensorHandle phid, double *trigger);
/**
 * Sets the weight change trigger.
 * @param phid An attached phidget weight sensor handle.
 * @param trigger The change trigger.
 */
 int CPhidgetWeightSensor_setWeightChangeTrigger(CPhidgetWeightSensorHandle phid, double trigger);
/** @} */
#ifndef CPHIDGET_CONSTANTS
#define CPHIDGET_CONSTANTS

/** \defgroup phidconst Phidget Constants 
 * Various constants used throughout the library.
 * @{
 */

/** \name Phidget States
 * Returned by getStatus() functions
 * @{
 */
#define PHIDGET_ATTACHED				0x1 /**< Phidget attached */
#define PHIDGET_NOTATTACHED				0x0 /**< Phidget not attached */
/** @} */

/** \name Phidget Error Codes
 * Returned by all C API calls
 * @{
 */
#define	PHIDGET_ERROR_CODE_COUNT		18
#define EPHIDGET_OK						0	/**< Function completed successfully. */
#define EPHIDGET_NOTFOUND				1	/**< Phidget not found. "A Phidget matching the type and or serial number could not be found." This code is not currently used. */
#define EPHIDGET_NOMEMORY				2	/**< No memory. "Memory could not be allocated." */
#define EPHIDGET_UNEXPECTED				3	/**< Unexpected. "Unexpected Error. Contact Phidgets Inc. for support." */
#define EPHIDGET_INVALIDARG				4	/**< Invalid argument. "Invalid argument passed to function." */
#define EPHIDGET_NOTATTACHED			5	/**< Phidget not attached. "Phidget not physically attached." */
#define EPHIDGET_INTERRUPTED			6	/**< Interrupted. "Read/Write operation was interrupted." This code is not currently used. */
#define EPHIDGET_INVALID				7	/**< Invalid error code. "The Error Code is not defined." */
#define EPHIDGET_NETWORK				8	/**< Network. "Network Error." */
#define EPHIDGET_UNKNOWNVAL				9	/**< Value unknown. "Value is Unknown (State not yet received from device, or not yet set by user)." */
#define EPHIDGET_BADPASSWORD			10	/**< Authorization exception. "Authorization Failed." */
#define EPHIDGET_UNSUPPORTED			11	/**< Unsupported. "Not Supported." */
#define EPHIDGET_DUPLICATE				12	/**< Duplicate request. "Duplicated request." */
#define EPHIDGET_TIMEOUT				13	/**< Timeout. "Given timeout has been exceeded." */
#define EPHIDGET_OUTOFBOUNDS			14	/**< Out of bounds. "Index out of Bounds." */
#define EPHIDGET_EVENT					15	/**< Event. "A non-null error code was returned from an event handler." This code is not currently used. */
#define EPHIDGET_NETWORK_NOTCONNECTED	16	/**< Network not connected. "A connection to the server does not exist." */
#define EPHIDGET_WRONGDEVICE			17	/**< Wrong device. "Function is not applicable for this device." */
/** @} */

/** \name Phidget Unknown Constants
 * Data values will be set to these constants when a call fails with \ref EPHIDGET_UNKNOWNVAL.
 * @{
 */
#define PUNK_BOOL	0x02					/**< Unknown Boolean (unsigned char) */
#define PUNK_INT	0x7FFFFFFF				/**< Unknown Integer (32-bit) */
#define PUNK_INT64	0x7FFFFFFFFFFFFFFFLL	/**< Unknown Integer (64-bit) */
#define PUNK_DBL	1e300					/**< Unknown Double */
#define PUNK_FLT	1e30					/**< Unknown Float */
/** @} */

#define PFALSE		0x00	/**< False. Used for boolean values. */
#define PTRUE		0x01	/**< True. Used for boolean values. */

/** @} */

#endif

#ifdef __cplusplus
}
#endif
#endif
