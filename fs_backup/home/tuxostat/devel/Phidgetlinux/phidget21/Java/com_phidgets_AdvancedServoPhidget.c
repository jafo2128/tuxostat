#include "../stdafx.h"
#include "phidget_jni.h"
#include "com_phidgets_AdvancedServoPhidget.h"
#include "../cphidgetadvancedservo.h"

EVENT_VARS(servoPositionChange, ServoPositionChange)
EVENT_VARS(servoVelocityChange, ServoVelocityChange)
EVENT_VARS(currentChange, CurrentChange)

JNI_LOAD(advservo, AdvancedServo)
	EVENT_VAR_SETUP(advservo, servoPositionChange, ServoPositionChange, ID, V)
	EVENT_VAR_SETUP(advservo, servoVelocityChange, ServoVelocityChange, ID, V)
	EVENT_VAR_SETUP(advservo, currentChange, CurrentChange, ID, V)
}

EVENT_HANDLER_INDEXED(AdvancedServo, servoPositionChange, ServoPositionChange, 
					  CPhidgetAdvancedServo_set_OnPositionChange_Handler, double)
EVENT_HANDLER_INDEXED(AdvancedServo, servoVelocityChange, ServoVelocityChange, 
					  CPhidgetAdvancedServo_set_OnVelocityChange_Handler, double)
EVENT_HANDLER_INDEXED(AdvancedServo, currentChange, CurrentChange, 
					  CPhidgetAdvancedServo_set_OnCurrentChange_Handler, double)

JNI_CREATE(AdvancedServo)
JNI_INDEXED_GETFUNC(AdvancedServo, Acceleration, Acceleration, jdouble)
JNI_INDEXED_SETFUNC(AdvancedServo, Acceleration, Acceleration, jdouble)
JNI_INDEXED_GETFUNC(AdvancedServo, AccelerationMin, AccelerationMin, jdouble)
JNI_INDEXED_GETFUNC(AdvancedServo, AccelerationMax, AccelerationMax, jdouble)
JNI_INDEXED_GETFUNC(AdvancedServo, VelocityLimit, VelocityLimit, jdouble)
JNI_INDEXED_SETFUNC(AdvancedServo, VelocityLimit, VelocityLimit, jdouble)
JNI_INDEXED_GETFUNC(AdvancedServo, Velocity, Velocity, jdouble)
JNI_INDEXED_GETFUNC(AdvancedServo, VelocityMin, VelocityMin, jdouble)
JNI_INDEXED_GETFUNC(AdvancedServo, VelocityMax, VelocityMax, jdouble)
JNI_INDEXED_GETFUNC(AdvancedServo, Position, Position, jdouble)
JNI_INDEXED_SETFUNC(AdvancedServo, Position, Position, jdouble)
JNI_INDEXED_GETFUNC(AdvancedServo, PositionMin, PositionMin, jdouble)
JNI_INDEXED_GETFUNC(AdvancedServo, PositionMax, PositionMax, jdouble)
JNI_INDEXED_SETFUNC(AdvancedServo, PositionMin, PositionMin, jdouble)
JNI_INDEXED_SETFUNC(AdvancedServo, PositionMax, PositionMax, jdouble)
JNI_INDEXED_GETFUNCBOOL(AdvancedServo, Engaged, Engaged)
JNI_INDEXED_SETFUNC(AdvancedServo, Engaged, Engaged, jboolean)
JNI_INDEXED_GETFUNCBOOL(AdvancedServo, Stopped, Stopped)
JNI_INDEXED_GETFUNCBOOL(AdvancedServo, SpeedRampingOn, SpeedRampingOn)
JNI_INDEXED_SETFUNC(AdvancedServo, SpeedRampingOn, SpeedRampingOn, jboolean)
JNI_INDEXED_GETFUNC(AdvancedServo, Current, Current, jdouble)
JNI_GETFUNC(AdvancedServo, MotorCount, MotorCount, jint)
