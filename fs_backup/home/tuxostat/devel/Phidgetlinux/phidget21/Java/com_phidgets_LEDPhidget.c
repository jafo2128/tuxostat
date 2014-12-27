#include "../stdafx.h"
#include "phidget_jni.h"
#include "com_phidgets_LEDPhidget.h"
#include "../cphidgetled.h"

JNI_LOAD(accel, LED)
}

JNI_CREATE(LED)
JNI_INDEXED_GETFUNC(LED, DiscreteLED, DiscreteLED, jint)
JNI_INDEXED_SETFUNC(LED, DiscreteLED, DiscreteLED, jint)
JNI_GETFUNC(LED, LEDCount, LEDCount, jint)
