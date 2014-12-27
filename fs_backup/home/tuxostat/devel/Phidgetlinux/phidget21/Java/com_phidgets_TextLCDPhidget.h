/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_phidgets_TextLCDPhidget */

#ifndef _Included_com_phidgets_TextLCDPhidget
#define _Included_com_phidgets_TextLCDPhidget
#ifdef __cplusplus
extern "C" {
#endif
#undef com_phidgets_TextLCDPhidget_PHIDGET_LOG_CRITICAL
#define com_phidgets_TextLCDPhidget_PHIDGET_LOG_CRITICAL 1L
#undef com_phidgets_TextLCDPhidget_PHIDGET_LOG_ERROR
#define com_phidgets_TextLCDPhidget_PHIDGET_LOG_ERROR 2L
#undef com_phidgets_TextLCDPhidget_PHIDGET_LOG_WARNING
#define com_phidgets_TextLCDPhidget_PHIDGET_LOG_WARNING 3L
#undef com_phidgets_TextLCDPhidget_PHIDGET_LOG_DEBUG
#define com_phidgets_TextLCDPhidget_PHIDGET_LOG_DEBUG 4L
#undef com_phidgets_TextLCDPhidget_PHIDGET_LOG_INFO
#define com_phidgets_TextLCDPhidget_PHIDGET_LOG_INFO 5L
#undef com_phidgets_TextLCDPhidget_PHIDGET_LOG_VERBOSE
#define com_phidgets_TextLCDPhidget_PHIDGET_LOG_VERBOSE 6L
/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    create
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_phidgets_TextLCDPhidget_create
  (JNIEnv *, jclass);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    getRowCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_TextLCDPhidget_getRowCount
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    getColumnCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_TextLCDPhidget_getColumnCount
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    getContrast
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_phidgets_TextLCDPhidget_getContrast
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    setContrast
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_TextLCDPhidget_setContrast
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    getBacklight
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_phidgets_TextLCDPhidget_getBacklight
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    setBacklight
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_TextLCDPhidget_setBacklight
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    getCursor
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_phidgets_TextLCDPhidget_getCursor
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    setCursor
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_TextLCDPhidget_setCursor
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    getCursorBlink
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_phidgets_TextLCDPhidget_getCursorBlink
  (JNIEnv *, jobject);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    setCursorBlink
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_TextLCDPhidget_setCursorBlink
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    setDisplayString
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_TextLCDPhidget_setDisplayString
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     com_phidgets_TextLCDPhidget
 * Method:    setCustomCharacter
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_com_phidgets_TextLCDPhidget_setCustomCharacter
  (JNIEnv *, jobject, jint, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
