#include "../stdafx.h"
#include <stdlib.h>
#include "com_phidgets_Manager.h"
#include "../cphidgetmanager.h"
#include "../csocket.h"
#include "phidget_jni.h"

static int CCONV attach_handler(CPhidgetHandle h, void *arg);
static int CCONV detach_handler(CPhidgetHandle h, void *arg);
static int CCONV serverConnect_handler(CPhidgetManagerHandle h, void *arg);
static int CCONV serverDisconnect_handler(CPhidgetManagerHandle h, void *arg);

static jfieldID manager_handle_fid;
static jfieldID nativeAttachHandler_fid;
static jfieldID nativeDetachHandler_fid;
static jfieldID nativeServerConnectHandler_fid;
static jfieldID nativeServerDisconnectHandler_fid;
static jmethodID attachEvent_cons;
static jmethodID detachEvent_cons;
static jmethodID serverConnectEvent_cons;
static jmethodID serverDisconnectEvent_cons;
static jmethodID phidget_cons;
static jmethodID fireAttach_mid;
static jmethodID fireDetach_mid;
static jmethodID fireServerConnect_mid;
static jmethodID fireServerDisconnect_mid;
static jclass attachEvent_class;
static jclass detachEvent_class;
static jclass serverConnectEvent_class;
static jclass serverDisconnectEvent_class;
static jclass manager_class;
static jclass phidget_class;

void
com_phidgets_Manager_OnLoad(JNIEnv *env)
{
	if (!(phidget_class = (*env)->FindClass(env, "com/phidgets/Phidget")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(phidget_class = (jclass)(*env)->NewGlobalRef(env, phidget_class)))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(manager_class = (*env)->FindClass(env, "com/phidgets/Manager")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(manager_class = (jclass)(*env)->NewGlobalRef(env, manager_class)))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(attachEvent_class = (*env)->FindClass(env, 
	  "com/phidgets/event/AttachEvent")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(attachEvent_class = (jclass)(*env)->NewGlobalRef(env, attachEvent_class)))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(detachEvent_class = (*env)->FindClass(env, 
	  "com/phidgets/event/DetachEvent")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(detachEvent_class = (jclass)(*env)->NewGlobalRef(env, detachEvent_class)))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(serverConnectEvent_class = (*env)->FindClass(env, 
	  "com/phidgets/event/ServerConnectEvent")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(serverConnectEvent_class = (jclass)(*env)->NewGlobalRef(env, serverConnectEvent_class)))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(serverDisconnectEvent_class = (*env)->FindClass(env, 
	  "com/phidgets/event/ServerDisconnectEvent")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(serverDisconnectEvent_class = (jclass)(*env)->NewGlobalRef(env, serverDisconnectEvent_class)))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(fireAttach_mid = (*env)->GetMethodID(env, manager_class,
	  "fireAttach", "(Lcom/phidgets/event/AttachEvent;)V")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(fireDetach_mid = (*env)->GetMethodID(env, manager_class,
	  "fireDetach", "(Lcom/phidgets/event/DetachEvent;)V")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(fireServerConnect_mid = (*env)->GetMethodID(env, manager_class,
	  "fireServerConnect", "(Lcom/phidgets/event/ServerConnectEvent;)V")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(fireServerDisconnect_mid = (*env)->GetMethodID(env, manager_class,
	  "fireServerDisconnect", "(Lcom/phidgets/event/ServerDisconnectEvent;)V")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(attachEvent_cons = (*env)->GetMethodID(env, attachEvent_class,
	  "<init>", "(Lcom/phidgets/Phidget;)V")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(detachEvent_cons = (*env)->GetMethodID(env, detachEvent_class,
	  "<init>", "(Lcom/phidgets/Phidget;)V")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(serverConnectEvent_cons = (*env)->GetMethodID(env, serverConnectEvent_class,
	  "<init>", "(Ljava/lang/Object;)V")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(serverDisconnectEvent_cons = (*env)->GetMethodID(env, serverDisconnectEvent_class,
	  "<init>", "(Ljava/lang/Object;)V")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(phidget_cons = (*env)->GetMethodID(env, phidget_class,
	  "<init>", "(J)V")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(nativeAttachHandler_fid = (*env)->GetFieldID(env, manager_class,
	  "nativeAttachHandler", "J")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(nativeDetachHandler_fid = (*env)->GetFieldID(env, manager_class,
	  "nativeDetachHandler", "J")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(nativeServerConnectHandler_fid = (*env)->GetFieldID(env, manager_class,
	  "nativeServerConnectHandler", "J")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(nativeServerDisconnectHandler_fid = (*env)->GetFieldID(env, manager_class,
	  "nativeServerDisconnectHandler", "J")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
	if (!(manager_handle_fid = (*env)->GetFieldID(env, manager_class,
	  "handle", "J")))
	{
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		abort();
	}
}

JNIEXPORT jboolean JNICALL
Java_com_phidgets_Manager_isAttached(JNIEnv *env, jobject obj)
{
	CPhidgetHandle h = (CPhidgetHandle)(uintptr_t)(*env)->GetLongField(env,
	    obj, manager_handle_fid);
	int error;
	int s;
	
	if ((error = CPhidget_getDeviceStatus(h, &s)))
		PH_THROW(error);

	return s == PHIDGET_ATTACHED;
}

JNIEXPORT jboolean JNICALL
Java_com_phidgets_Manager_isAttachedToServer(JNIEnv *env, jobject obj)
{
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)(*env)->GetLongField(env,
	    obj, manager_handle_fid);
	int error;
	int s;
	
	if ((error = CPhidgetManager_getServerStatus(h, &s)))
		PH_THROW(error);

	return s == PHIDGET_ATTACHED;
}

JNIEXPORT jint JNICALL
Java_com_phidgets_Manager_getServerPort(JNIEnv *env, jobject obj)
{
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)(*env)->GetLongField(env,
	    obj, manager_handle_fid);
	int error;
	int port;
	const char *addr;

	if ((error = CPhidgetManager_getServerAddress(h, &addr, &port)))
		PH_THROW(error);

	return port;
}

JNIEXPORT jstring JNICALL
Java_com_phidgets_Manager_getServerAddress(JNIEnv *env, jobject obj)
{
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)(*env)->GetLongField(env,
	    obj, manager_handle_fid);
	int error;
	int port;
	const char *addr;

	if ((error = CPhidgetManager_getServerAddress(h, &addr, &port)))
		PH_THROW(error);

	return (*env)->NewStringUTF(env, addr);
}

JNIEXPORT jstring JNICALL
Java_com_phidgets_Manager_getServerID(JNIEnv *env, jobject obj)
{
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)(*env)->GetLongField(env,
	    obj, manager_handle_fid);
	int error;
	const char *id;

	if ((error = CPhidgetManager_getServerID(h, &id)))
		PH_THROW(error);

	return (*env)->NewStringUTF(env, id);
}

JNIEXPORT void JNICALL
Java_com_phidgets_Manager_enableManagerAttachEvents(JNIEnv *env, jobject obj,
  jboolean b)
{
	jlong gr = updateGlobalRef(env, obj, nativeAttachHandler_fid, b);
	
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)
	    (*env)->GetLongField(env, obj, manager_handle_fid);

	CPhidgetManager_set_OnAttach_Handler(h, b ? attach_handler : 0,
	    (void *)(uintptr_t)gr);
}

static int CCONV
attach_handler(CPhidgetHandle h, void *arg)
{
	JNIEnv *env;
	jobject obj;
	jobject device;
	jobject attachEvent;
	jlong devhandle;

	if ((*ph_vm)->AttachCurrentThread(ph_vm, (void **)&env, NULL))
		abort();

	obj = (jobject)arg;
	devhandle = (jlong)(uintptr_t)h;

	if (!(device = (*env)->NewObject(env, phidget_class, phidget_cons,
	  devhandle)))
		return -1;

	if (!(attachEvent = (*env)->NewObject(env, attachEvent_class, attachEvent_cons,
	  device)))
		return -1;
	(*env)->CallVoidMethod(env, obj, fireAttach_mid, attachEvent);
	(*env)->DeleteLocalRef(env, device);
	(*env)->DeleteLocalRef(env, attachEvent);
	(*ph_vm)->DetachCurrentThread(ph_vm);

	return 0;
}

JNIEXPORT void JNICALL
Java_com_phidgets_Manager_enableManagerDetachEvents(JNIEnv *env, jobject obj,
  jboolean b)
{
	jlong gr = updateGlobalRef(env, obj, nativeDetachHandler_fid, b);
	
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)
	    (*env)->GetLongField(env, obj, manager_handle_fid);

	CPhidgetManager_set_OnDetach_Handler(h, b ? detach_handler : 0, (void *)(uintptr_t)gr);
}

static int CCONV
detach_handler(CPhidgetHandle h, void *arg)
{
	JNIEnv *env;
	jobject obj;
	jobject device;
	jobject detachEvent;
	jlong devhandle;

	if ((*ph_vm)->AttachCurrentThread(ph_vm, (void **)&env, NULL))
		abort();

	obj = (jobject)arg;
	devhandle = (jlong)(uintptr_t)h;

	if (!(device = (*env)->NewObject(env, phidget_class, phidget_cons,
	  devhandle)))
		return -1;

	if (!(detachEvent = (*env)->NewObject(env, detachEvent_class, detachEvent_cons,
	  device)))
		return -1;
	(*env)->CallVoidMethod(env, obj, fireDetach_mid, detachEvent);
	(*env)->DeleteLocalRef(env, device);
	(*env)->DeleteLocalRef(env, detachEvent);
	(*ph_vm)->DetachCurrentThread(ph_vm);

	return 0;
}

JNIEXPORT void JNICALL
Java_com_phidgets_Manager_enableServerConnectEvents(JNIEnv *env, jobject obj,
  jboolean b)
{
	jlong gr = updateGlobalRef(env, obj, nativeServerConnectHandler_fid, b);
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)(*env)->GetLongField(env,
	    obj, manager_handle_fid);

	CPhidgetManager_set_OnServerConnect_Handler(h, b ? serverConnect_handler : 0,
	    (void *)(uintptr_t)gr);
}

static int CCONV
serverConnect_handler(CPhidgetManagerHandle h, void *arg)
{
	JNIEnv *env;
	jobject obj;
	jobject serverConnectEvent;
	jint result;

	result = (*ph_vm)->GetEnv(ph_vm, (void **)&env, JNI_VERSION_1_4);
	
	if(result == JNI_EDETACHED)
	{
	if ((*ph_vm)->AttachCurrentThread(ph_vm, (void **)&env, NULL))
		abort();
	}

	obj = (jobject)arg;

	if (!(serverConnectEvent = (*env)->NewObject(env, serverConnectEvent_class, serverConnectEvent_cons,
	  obj)))
		return -1;
	(*env)->CallVoidMethod(env, obj, fireServerConnect_mid, serverConnectEvent);
	(*env)->DeleteLocalRef(env, serverConnectEvent);
	(*ph_vm)->DetachCurrentThread(ph_vm);

	return 0;
}

JNIEXPORT void JNICALL
Java_com_phidgets_Manager_enableServerDisconnectEvents(JNIEnv *env, jobject obj,
  jboolean b)
{
	jlong gr = updateGlobalRef(env, obj, nativeServerDisconnectHandler_fid, b);
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)(*env)->GetLongField(env,
	    obj, manager_handle_fid);

	CPhidgetManager_set_OnServerDisconnect_Handler(h, b ? serverDisconnect_handler : 0,
	    (void *)(uintptr_t)gr);
}

static int CCONV
serverDisconnect_handler(CPhidgetManagerHandle h, void *arg)
{
	JNIEnv *env;
	jobject obj;
	jobject serverDisconnectEvent;
	jint result;

	result = (*ph_vm)->GetEnv(ph_vm, (void **)&env, JNI_VERSION_1_4);
	
	if(result == JNI_EDETACHED)
	{
	if ((*ph_vm)->AttachCurrentThread(ph_vm, (void **)&env, NULL))
		abort();
	}

	obj = (jobject)arg;

	if (!(serverDisconnectEvent = (*env)->NewObject(env, serverDisconnectEvent_class, serverDisconnectEvent_cons,
	  obj)))
		return -1;
	(*env)->CallVoidMethod(env, obj, fireServerDisconnect_mid, serverDisconnectEvent);
	(*env)->DeleteLocalRef(env, serverDisconnectEvent);
	(*ph_vm)->DetachCurrentThread(ph_vm);

	return 0;
}

JNIEXPORT void JNICALL
Java_com_phidgets_Manager_nativeOpen(JNIEnv *env, jobject obj)
{
	int error;
	
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)
	    (*env)->GetLongField(env, obj, manager_handle_fid);

	if ((error = CPhidgetManager_open(h)))
		PH_THROW(error);
}

JNIEXPORT void JNICALL
Java_com_phidgets_Manager_nativeOpenRemote(JNIEnv *env, jobject obj, jstring serverID, jstring pass)
{
	int error;
    jboolean iscopy;
    const char *serverIDcopy = serverID?(*env)->GetStringUTFChars(
				env, serverID, &iscopy):0;
    const char *passcopy = (*env)->GetStringUTFChars(
                env, pass, &iscopy);

	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)
	    (*env)->GetLongField(env, obj, manager_handle_fid);

	if ((error = CPhidgetManager_openRemote(h, serverIDcopy, passcopy)))
		PH_THROW(error);
}

JNIEXPORT void JNICALL
Java_com_phidgets_Manager_nativeOpenRemoteIP(JNIEnv *env, jobject obj, jstring ipAddr, jint port, jstring pass)
{
	int error;
    jboolean iscopy;
    const char *ipAddrcopy = (*env)->GetStringUTFChars(
                env, ipAddr, &iscopy);
    const char *passcopy = (*env)->GetStringUTFChars(
                env, pass, &iscopy);

	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)
	    (*env)->GetLongField(env, obj, manager_handle_fid);

	if ((error = CPhidgetManager_openRemoteIP(h, ipAddrcopy, port, passcopy)))
		PH_THROW(error);
}

JNIEXPORT void JNICALL
Java_com_phidgets_Manager_nativeDelete(JNIEnv *env, jobject obj)
{
	int error;
	
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)
	    (*env)->GetLongField(env, obj, manager_handle_fid);

	if ((error = CPhidgetManager_delete(h)))
		PH_THROW(error);
}

JNIEXPORT void JNICALL
Java_com_phidgets_Manager_nativeClose(JNIEnv *env, jobject obj)
{
	int error;
	
	CPhidgetManagerHandle h = (CPhidgetManagerHandle)(uintptr_t)
	    (*env)->GetLongField(env, obj, manager_handle_fid);

	if ((error = CPhidgetManager_close(h)))
		PH_THROW(error);
}

JNIEXPORT jlong JNICALL
Java_com_phidgets_Manager_create(JNIEnv *env, jobject obj)
{
	int error;
	CPhidgetManagerHandle phidm;
	
	if ((error = CPhidgetManager_create(&phidm)))
		PH_THROW(error);
	return (jlong)(uintptr_t)phidm; 
}
