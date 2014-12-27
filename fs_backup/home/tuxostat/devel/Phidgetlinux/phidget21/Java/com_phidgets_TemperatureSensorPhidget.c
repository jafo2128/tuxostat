#include "../stdafx.h"
#include "phidget_jni.h"
#include "com_phidgets_TemperatureSensorPhidget.h"
#include "../cphidgettemperaturesensor.h"

EVENT_VARS(temperatureChange, TemperatureChange)

JNI_LOAD(temp, TemperatureSensor)
	EVENT_VAR_SETUP(temp, temperatureChange, TemperatureChange, ID, V)
}

EVENT_HANDLER_INDEXED(TemperatureSensor, temperatureChange, TemperatureChange, 
					  CPhidgetTemperatureSensor_set_OnTemperatureChange_Handler, double)

JNI_CREATE(TemperatureSensor)
JNI_INDEXED_GETFUNC(TemperatureSensor, TemperatureChangeTrigger, TemperatureChangeTrigger, jdouble)
JNI_INDEXED_SETFUNC(TemperatureSensor, TemperatureChangeTrigger, TemperatureChangeTrigger, jdouble)
JNI_INDEXED_GETFUNC(TemperatureSensor, ThermocoupleType, ThermocoupleType, jint)
JNI_INDEXED_SETFUNC(TemperatureSensor, ThermocoupleType, ThermocoupleType, jint)
JNI_INDEXED_GETFUNC(TemperatureSensor, Temperature, Temperature, jdouble)
JNI_INDEXED_GETFUNC(TemperatureSensor, TemperatureMin, TemperatureMin, jdouble)
JNI_INDEXED_GETFUNC(TemperatureSensor, TemperatureMax, TemperatureMax, jdouble)
JNI_INDEXED_GETFUNC(TemperatureSensor, Potential, Potential, jdouble)
JNI_INDEXED_GETFUNC(TemperatureSensor, PotentialMin, PotentialMin, jdouble)
JNI_INDEXED_GETFUNC(TemperatureSensor, PotentialMax, PotentialMax, jdouble)
JNI_GETFUNC(TemperatureSensor, AmbientTemperature, AmbientTemperature, jdouble)
JNI_GETFUNC(TemperatureSensor, AmbientTemperatureMin, AmbientTemperatureMin, jdouble)
JNI_GETFUNC(TemperatureSensor, AmbientTemperatureMax, AmbientTemperatureMax, jdouble)
JNI_GETFUNC(TemperatureSensor, TemperatureInputCount, TemperatureInputCount, jint)

//Deprecated
JNI_GETFUNC(TemperatureSensor, SensorCount, TemperatureInputCount, jint)
