#include "../stdafx.h"
#include "phidget_jni.h"
#include "com_phidgets_TextLCDPhidget.h"
#include "../cphidgettextlcd.h"

JNI_LOAD(lcd, TextLCD)
}

JNI_CREATE(TextLCD)
JNI_GETFUNCBOOL(TextLCD, Backlight, Backlight)
JNI_SETFUNC(TextLCD, Backlight, Backlight, jboolean)
JNI_GETFUNC(TextLCD, Contrast, Contrast, jint)
JNI_SETFUNC(TextLCD, Contrast, Contrast, jint)
JNI_GETFUNCBOOL(TextLCD, Cursor, CursorOn)
JNI_SETFUNC(TextLCD, Cursor, CursorOn, jboolean)
JNI_GETFUNCBOOL(TextLCD, CursorBlink, CursorBlink)
JNI_SETFUNC(TextLCD, CursorBlink, CursorBlink, jboolean)
JNI_GETFUNC(TextLCD, RowCount, RowCount, jint)
JNI_GETFUNC(TextLCD, ColumnCount, ColumnCount, jint)

JNIEXPORT void JNICALL
Java_com_phidgets_TextLCDPhidget_setDisplayString(JNIEnv *env, jobject obj, jint index, jstring v)
{
	CPhidgetTextLCDHandle h = (CPhidgetTextLCDHandle)(uintptr_t)
	    (*env)->GetLongField(env, obj, handle_fid);
	int error;
    jboolean iscopy;
    const char *textString = (*env)->GetStringUTFChars(
                env, v, &iscopy);
	if ((error = CPhidgetTextLCD_setDisplayString(h, index, (char *)textString)))
		PH_THROW(error);
	(*env)->ReleaseStringUTFChars(env, v, textString);
}

JNIEXPORT void JNICALL
Java_com_phidgets_TextLCDPhidget_setCustomCharacter(JNIEnv *env, jobject obj, jint index, jint v, jint v2)
{
	CPhidgetTextLCDHandle h = (CPhidgetTextLCDHandle)(uintptr_t)
	    (*env)->GetLongField(env, obj, handle_fid);
	int error;
	if ((error = CPhidgetTextLCD_setCustomCharacter(h, index, v, v2)))
		PH_THROW(error);
}
