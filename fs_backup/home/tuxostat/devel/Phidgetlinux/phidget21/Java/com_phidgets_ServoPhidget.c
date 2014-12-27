#include "../stdafx.h"
#include "phidget_jni.h"
#include "com_phidgets_ServoPhidget.h"
#include "../cphidgetservo.h"

EVENT_VARS(servoPositionChange, ServoPositionChange)

JNI_LOAD(servo, Servo)
	EVENT_VAR_SETUP(servo, servoPositionChange, ServoPositionChange, ID, V)
}

EVENT_HANDLER_INDEXED(Servo, servoPositionChange, ServoPositionChange, 
					  CPhidgetServo_set_OnPositionChange_Handler, double)

JNI_CREATE(Servo)
JNI_INDEXED_GETFUNC(Servo, Position, Position, jdouble)
JNI_INDEXED_SETFUNC(Servo, Position, Position, jdouble)
JNI_INDEXED_GETFUNC(Servo, PositionMin, PositionMin, jdouble)
JNI_INDEXED_GETFUNC(Servo, PositionMax, PositionMax, jdouble)
JNI_INDEXED_GETFUNCBOOL(Servo, Engaged, Engaged)
JNI_INDEXED_SETFUNC(Servo, Engaged, Engaged, jboolean)
JNI_GETFUNC(Servo, MotorCount, MotorCount, jint)

//Deprecated
JNI_INDEXED_GETFUNCBOOL(Servo, MotorOn, Engaged)
JNI_INDEXED_SETFUNC(Servo, MotorOn, Engaged, jboolean)
