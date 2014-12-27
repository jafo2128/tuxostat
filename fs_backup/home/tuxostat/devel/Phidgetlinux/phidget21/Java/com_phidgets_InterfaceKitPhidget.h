/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_phidgets_InterfaceKitPhidget */

#ifndef _Included_com_phidgets_InterfaceKitPhidget
#define _Included_com_phidgets_InterfaceKitPhidget
#ifdef __cplusplus
extern "C" {
#endif
#undef com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_CRITICAL
#define com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_CRITICAL 1L
#undef com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_ERROR
#define com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_ERROR 2L
#undef com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_WARNING
#define com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_WARNING 3L
#undef com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_DEBUG
#define com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_DEBUG 4L
#undef com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_INFO
#define com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_INFO 5L
#undef com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_VERBOSE
#define com_phidgets_InterfaceKitPhidget_PHIDGET_LOG_VERBOSE 6L
/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    create
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_phidgets_InterfaceKitPhidget_create
  (JNIEnv *, jclass);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    getOutputCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_InterfaceKitPhidget_getOutputCount
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    getInputCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_InterfaceKitPhidget_getInputCount
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    getSensorCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_InterfaceKitPhidget_getSensorCount
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    getInputState
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_phidgets_InterfaceKitPhidget_getInputState
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    getOutputState
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_phidgets_InterfaceKitPhidget_getOutputState
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    getSensorValue
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_InterfaceKitPhidget_getSensorValue
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    getSensorRawValue
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_InterfaceKitPhidget_getSensorRawValue
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    getSensorChangeTrigger
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_InterfaceKitPhidget_getSensorChangeTrigger
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    getRatiometric
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_phidgets_InterfaceKitPhidget_getRatiometric
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    setOutputState
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_InterfaceKitPhidget_setOutputState
  (JNIEnv *, jobject, jint, jboolean);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    setSensorChangeTrigger
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_InterfaceKitPhidget_setSensorChangeTrigger
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    setRatiometric
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_InterfaceKitPhidget_setRatiometric
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    enableInputChangeEvents
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_InterfaceKitPhidget_enableInputChangeEvents
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    enableOutputChangeEvents
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_InterfaceKitPhidget_enableOutputChangeEvents
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_phidgets_InterfaceKitPhidget
 * Method:    enableSensorChangeEvents
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_InterfaceKitPhidget_enableSensorChangeEvents
  (JNIEnv *, jobject, jboolean);

#ifdef __cplusplus
}
#endif
#endif