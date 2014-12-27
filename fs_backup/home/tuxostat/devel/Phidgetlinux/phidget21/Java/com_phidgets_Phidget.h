/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_phidgets_Phidget */

#ifndef _Included_com_phidgets_Phidget
#define _Included_com_phidgets_Phidget
#ifdef __cplusplus
extern "C" {
#endif
#undef com_phidgets_Phidget_PHIDGET_LOG_CRITICAL
#define com_phidgets_Phidget_PHIDGET_LOG_CRITICAL 1L
#undef com_phidgets_Phidget_PHIDGET_LOG_ERROR
#define com_phidgets_Phidget_PHIDGET_LOG_ERROR 2L
#undef com_phidgets_Phidget_PHIDGET_LOG_WARNING
#define com_phidgets_Phidget_PHIDGET_LOG_WARNING 3L
#undef com_phidgets_Phidget_PHIDGET_LOG_DEBUG
#define com_phidgets_Phidget_PHIDGET_LOG_DEBUG 4L
#undef com_phidgets_Phidget_PHIDGET_LOG_INFO
#define com_phidgets_Phidget_PHIDGET_LOG_INFO 5L
#undef com_phidgets_Phidget_PHIDGET_LOG_VERBOSE
#define com_phidgets_Phidget_PHIDGET_LOG_VERBOSE 6L
/*
 * Class:     com_phidgets_Phidget
 * Method:    getLibraryVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_phidgets_Phidget_getLibraryVersion
  (JNIEnv *, jclass);

/*
 * Class:     com_phidgets_Phidget
 * Method:    nativeEnableLogging
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_nativeEnableLogging
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     com_phidgets_Phidget
 * Method:    disableLogging
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_disableLogging
  (JNIEnv *, jclass);

/*
 * Class:     com_phidgets_Phidget
 * Method:    nativeLog
 * Signature: (ILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_nativeLog
  (JNIEnv *, jclass, jint, jstring, jstring);

/*
 * Class:     com_phidgets_Phidget
 * Method:    getSerialNumber
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_Phidget_getSerialNumber
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    getDeviceVersion
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_Phidget_getDeviceVersion
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    getDeviceType
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_phidgets_Phidget_getDeviceType
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    getDeviceName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_phidgets_Phidget_getDeviceName
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    isAttached
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_phidgets_Phidget_isAttached
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    isAttachedToServer
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_phidgets_Phidget_isAttachedToServer
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    getDeviceLabel
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_phidgets_Phidget_getDeviceLabel
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    setDeviceLabel
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_setDeviceLabel
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_phidgets_Phidget
 * Method:    getServerAddress
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_phidgets_Phidget_getServerAddress
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    getServerID
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_phidgets_Phidget_getServerID
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    getServerPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_Phidget_getServerPort
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    nativeClose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_nativeClose
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    nativeDelete
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_nativeDelete
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_Phidget
 * Method:    nativeOpen
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_nativeOpen
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_phidgets_Phidget
 * Method:    nativeOpenRemote
 * Signature: (ILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_nativeOpenRemote
  (JNIEnv *, jobject, jint, jstring, jstring);

/*
 * Class:     com_phidgets_Phidget
 * Method:    nativeOpenRemoteIP
 * Signature: (ILjava/lang/String;ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_nativeOpenRemoteIP
  (JNIEnv *, jobject, jint, jstring, jint, jstring);

/*
 * Class:     com_phidgets_Phidget
 * Method:    waitForAttachment
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_waitForAttachment
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_phidgets_Phidget
 * Method:    enableAttachEvents
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_enableAttachEvents
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_phidgets_Phidget
 * Method:    enableErrorEvents
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_enableErrorEvents
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_phidgets_Phidget
 * Method:    enableDetachEvents
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_enableDetachEvents
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_phidgets_Phidget
 * Method:    enableServerConnectEvents
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_enableServerConnectEvents
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_phidgets_Phidget
 * Method:    enableServerDisconnectEvents
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_Phidget_enableServerDisconnectEvents
  (JNIEnv *, jobject, jboolean);

#ifdef __cplusplus
}
#endif
#endif
