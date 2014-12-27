/* All event handlers for the various devices */
#include "stdafx.h"
#include "phidgetinterface.h"
#include "cphidgetlist.h"
#include "pdictserver.h"
#include "pdict.h"
#include "utils.h"
#include "eventhandlers.h"
#include "PhidgetWebservice21.h"
#include "zeroconf.h"

//regex_t phidgetex;

//Initial key setup
int phidget_accelerometer_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetAccelerometerHandle phid = (CPhidgetAccelerometerHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfAxes", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.accelerometer.numAxis);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AccelerationMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->accelerationMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AccelerationMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->accelerationMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;
	return EPHIDGET_OK;
}
int phidget_advancedservo_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret, i=0;
	CPhidgetAdvancedServoHandle phid = (CPhidgetAdvancedServoHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfMotors", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.advancedservo.numMotors);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AccelerationMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->accelerationMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AccelerationMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->accelerationMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	
	for(i=0;i<phid->phid.attr.advancedservo.numMotors;i++)
	{
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PositionMin/%d", phid->phid.deviceType, phid->phid.serialNumber, i);
		snprintf(val, MAX_VAL_SIZE, "%lf", phid->motorPositionMin[i]);
		if((ret = add_key(pdss, key, val))) 
		{
			free(key); key = NULL;
			free(val); val = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PositionMax/%d", phid->phid.deviceType, phid->phid.serialNumber, i);
		snprintf(val, MAX_VAL_SIZE, "%lf", phid->motorPositionMax[i]);
		if((ret = add_key(pdss, key, val))) 
		{
			free(key); key = NULL;
			free(val); val = NULL;
			return ret;
		}
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/VelocityMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->velocityMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/VelocityMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->velocityMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_encoder_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetEncoderHandle phid = (CPhidgetEncoderHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfEncoders", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.encoder.numEncoders);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfInputs", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.encoder.numInputs);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_gps_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	return EPHIDGET_OK;
}
int phidget_gyroscope_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetGyroscopeHandle phid = (CPhidgetGyroscopeHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfAxes", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.gyroscope.numAxis);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_interfacekit_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetInterfaceKitHandle phid = (CPhidgetInterfaceKitHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfSensors", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.ifkit.numSensors);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfInputs", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.ifkit.numInputs);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfOutputs", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.ifkit.numOutputs);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_led_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetLEDHandle phid = (CPhidgetLEDHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfLEDs", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.led.numLEDs);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_motorcontrol_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetMotorControlHandle phid = (CPhidgetMotorControlHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfMotors", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.motorcontrol.numMotors);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfInputs", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.motorcontrol.numInputs);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AccelerationMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->accelerationMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AccelerationMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->accelerationMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_phsensor_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetPHSensorHandle phid = (CPhidgetPHSensorHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PHMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->phMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PHMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->phMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PotentialMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->potentialMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PotentialMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->potentialMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_rfid_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetRFIDHandle phid = (CPhidgetRFIDHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfOutputs", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.rfid.numOutputs);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/TagState", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->tagPresent);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_servo_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetServoHandle phid = (CPhidgetServoHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfMotors", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.servo.numMotors);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PositionMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->motorPositionMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PositionMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->motorPositionMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_stepper_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetStepperHandle phid = (CPhidgetStepperHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfMotors", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.stepper.numMotors);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfInputs", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.stepper.numInputs);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PositionMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lld", phid->motorPositionMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PositionMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lld", phid->motorPositionMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AccelerationMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->accelerationMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AccelerationMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->accelerationMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/VelocityMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->motorSpeedMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/VelocityMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->motorSpeedMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/CurrentMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->currentMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/CurrentMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->currentMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_temperaturesensor_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret, i=0;
	CPhidgetTemperatureSensorHandle phid = (CPhidgetTemperatureSensorHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfSensors", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.temperaturesensor.numTempInputs);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PotentialMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->potentialMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PotentialMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->potentialMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AmbientTemperatureMin", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->ambientTemperatureMin);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AmbientTemperatureMax", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->ambientTemperatureMax);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	for(i=0;i<phid->phid.attr.temperaturesensor.numTempInputs;i++)
	{
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/TemperatureMin/%d", phid->phid.deviceType, phid->phid.serialNumber, i);
		snprintf(val, MAX_VAL_SIZE, "%lf", phid->temperatureMin[i]);
		if((ret = add_key(pdss, key, val))) 
		{
			free(key); key = NULL;
			free(val); val = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/TemperatureMax/%d", phid->phid.deviceType, phid->phid.serialNumber, i);
		snprintf(val, MAX_VAL_SIZE, "%lf", phid->temperatureMax[i]);
		if((ret = add_key(pdss, key, val))) 
		{
			free(key); key = NULL;
			free(val); val = NULL;
			return ret;
		}
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_textlcd_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetTextLCDHandle phid = (CPhidgetTextLCDHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfRows", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.textlcd.numRows);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfColumns", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.textlcd.numColumns);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_textled_initkeys(CPhidgetHandle arg, pds_session_t *pdss)
{
	int ret;
	CPhidgetTextLEDHandle phid = (CPhidgetTextLEDHandle)arg;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfRows", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.textled.numRows);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/NumberOfColumns", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->phid.attr.textled.numColumns);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return EPHIDGET_OK;
}
int phidget_weightsensor_initkeys(CPhidgetHandle phid, pds_session_t *pdss)
{
	return EPHIDGET_OK;
}

int(*fptrInitKeys[PHIDGET_DEVICE_TYPE_COUNT])(CPhidgetHandle phid, pds_session_t *pdss) = {NULL, NULL, NULL,
phidget_interfacekit_initkeys,
phidget_servo_initkeys,
NULL, NULL,
phidget_rfid_initkeys,
phidget_weightsensor_initkeys,NULL,
phidget_textlcd_initkeys,
phidget_motorcontrol_initkeys, NULL,
phidget_led_initkeys,
phidget_encoder_initkeys,
phidget_stepper_initkeys,
phidget_temperaturesensor_initkeys,
phidget_accelerometer_initkeys,
phidget_textled_initkeys,
phidget_phsensor_initkeys,
phidget_gyroscope_initkeys,
phidget_advancedservo_initkeys,
phidget_gps_initkeys};

//Events from Phidget21
int CCONV attach_handler(CPhidgetHandle phid, void *pdss)
{
	int ret;
	const char *name;
	char *key, *val;

	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Label", phid->deviceType, phid->serialNumber);
	if((ret = add_key(pdss, key, phid->label)))//phid->label[0]?phid->label:"(none)"))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Version", phid->deviceType, phid->serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%d", phid->deviceVersion);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Name", phid->deviceType, phid->serialNumber);
	CPhidget_getDeviceName(phid, &name);
	if((ret = add_key(pdss, key, name))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	if((ret = fptrInitKeys[phid->deviceID](phid, pdss)))
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Status", phid->deviceType, phid->serialNumber);
	snprintf(val, MAX_VAL_SIZE, "Attached");
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV detach_handler(CPhidgetHandle phid, void *pdss)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Status", phid->deviceType, phid->serialNumber);
	snprintf(val, MAX_VAL_SIZE, "Detached");
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "^/P[CS]K/%s/%d", phid->deviceType, phid->serialNumber);
	if((ret = remove_key(pdss, key)))
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;
	return 0;
}

int CCONV Accelerometer_AccelerationChange(CPhidgetAccelerometerHandle phid, void *pdss, int Index, double Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Acceleration/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV AdvancedServo_PositionChange(CPhidgetAdvancedServoHandle phid, void *pdss, int Index, double Val)
{
	int ret, stopped;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Position/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	CPhidgetAdvancedServo_getStopped(phid, Index, &stopped);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Stopped/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", stopped);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV AdvancedServo_VelocityChange(CPhidgetAdvancedServoHandle phid, void *pdss, int Index, double Val)
{
	int ret, stopped;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Velocity/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	CPhidgetAdvancedServo_getStopped(phid, Index, &stopped);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Stopped/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", stopped);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV AdvancedServo_CurrentChange(CPhidgetAdvancedServoHandle phid, void *pdss, int Index, double Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Current/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV Encoder_InputChange(CPhidgetEncoderHandle phid, void *pdss, int Index, int Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Input/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV Encoder_PositionChange(CPhidgetEncoderHandle phid, void *pdss, int Index, int Time, int PositionChange)
{
	int ret;
	int posn;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	CPhidgetEncoder_getPosition(phid, Index, &posn);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Position/%d/", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d/%d/%d", Time, PositionChange, posn);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV GPS_NMEAData(CPhidgetGPSHandle phid, void *pdss, const char *data)
{
	int ret;
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Data", phid->phid.deviceType, phid->phid.serialNumber);
	if((ret = add_key(pdss, key, data)))
	{
		free(key); key = NULL;
		return ret;
	}
	free(key); key = NULL;
	return 0;
}

int CCONV Gyroscope_AngularRateChange(CPhidgetGyroscopeHandle phid, void *pdss, int Index, double Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AngularRate/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV InterfaceKit_InputChange(CPhidgetInterfaceKitHandle phid, void *pdss, int Index, int Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Input/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV InterfaceKit_OutputChange(CPhidgetInterfaceKitHandle phid, void *pdss, int Index, int Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Output/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV InterfaceKit_SensorChange(CPhidgetInterfaceKitHandle phid, void *pdss, int Index, int Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Sensor/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV MotorControl_InputChange(CPhidgetMotorControlHandle phid, void *pdss, int Index, int Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Input/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV MotorControl_VelocityChange(CPhidgetMotorControlHandle phid, void *pdss, int Index, double Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Velocity/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV MotorControl_CurrentChange(CPhidgetMotorControlHandle phid, void *pdss, int Index, double Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Current/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV PHSensor_PHChange(CPhidgetPHSensorHandle phid, void *pdss, double Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PH", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Potential", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->Potential);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV RFID_Tag(CPhidgetRFIDHandle phid, void *pdss, unsigned char *Tag)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Tag", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%02x%02x%02x%02x%02x",Tag[0],Tag[1],Tag[2],Tag[3],Tag[4]);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/TagState", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "1");
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV RFID_TagLost(CPhidgetRFIDHandle phid, void *pdss, unsigned char *Tag)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/TagLoss", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%02x%02x%02x%02x%02x",Tag[0],Tag[1],Tag[2],Tag[3],Tag[4]);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}

	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/TagState", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "0");
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV RFID_OutputChange(CPhidgetRFIDHandle phid, void *pdss, int Index, int Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Output/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV Servo_PositionChange(CPhidgetServoHandle phid, void *pdss, int Index, double Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Position/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV Stepper_InputChange(CPhidgetStepperHandle phid, void *pdss, int Index, int Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Input/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV Stepper_PositionChange(CPhidgetStepperHandle phid, void *pdss, int Index, long long Val)
{
	int ret, stopped;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/CurrentPosition/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lld", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	CPhidgetStepper_getStopped(phid, Index, &stopped);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Stopped/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", stopped);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV Stepper_VelocityChange(CPhidgetStepperHandle phid, void *pdss, int Index, double Val)
{
	int ret, stopped;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Velocity/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	CPhidgetStepper_getStopped(phid, Index, &stopped);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Stopped/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%d", stopped);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV Stepper_CurrentChange(CPhidgetStepperHandle phid, void *pdss, int Index, double Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Current/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}


int CCONV TemperatureSensor_TemperatureChange(CPhidgetTemperatureSensorHandle phid, void *pdss, int Index, double Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Temperature/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Potential/%d", phid->phid.deviceType, phid->phid.serialNumber, Index);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->Potential[Index]);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AmbientTemperature", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", phid->AmbientTemperature);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

int CCONV WeightSensor_WeightChange(CPhidgetWeightSensorHandle phid, void *pdss, double Val)
{
	int ret;
	char *key, *val;
	key = (char *)malloc(MAX_KEY_SIZE);
	val = (char *)malloc(MAX_VAL_SIZE);
	snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Weight", phid->phid.deviceType, phid->phid.serialNumber);
	snprintf(val, MAX_VAL_SIZE, "%lf", Val);
	if((ret = add_key(pdss, key, val))) 
	{
		free(key); key = NULL;
		free(val); val = NULL;
		return ret;
	}
	free(key); key = NULL;
	free(val); val = NULL;

	return 0;
}

//Events from client (Remote Phidget21)

int phidget_accelerometer_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret;
	CPhidgetAccelerometerHandle accel = (CPhidgetAccelerometerHandle)phid;
	double value = strtod(state, NULL);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		if((ret = CPhidgetAccelerometer_setAccelerationChangeTrigger(accel, index, value)))
		{
			free(key); key = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Trigger/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for Accelerometer: %s\n", setThing);
	}
	free(key); key = NULL;
	return EPHIDGET_OK;
}

int phidget_advancedservo_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret;
	CPhidgetAdvancedServoHandle advservo = (CPhidgetAdvancedServoHandle)phid;
	double value = strtod(state, NULL);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Acceleration", sizeof("Acceleration")))
	{
		if((ret =  CPhidgetAdvancedServo_setAcceleration(advservo, index, value)))
		{
			free(key); key = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Acceleration/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "VelocityLimit", sizeof("VelocityLimit")))
	{
		if((ret = CPhidgetAdvancedServo_setVelocityLimit(advservo, index, value)))
		{
			free(key); key = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/VelocityLimit/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "PositionMin", sizeof("PositionMin")))
	{
		if((ret = CPhidgetAdvancedServo_setPositionMin(advservo, index, value)))
		{
			free(key); key = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PositionMin/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "PositionMax", sizeof("PositionMax")))
	{
		if((ret = CPhidgetAdvancedServo_setPositionMax(advservo, index, value)))
		{
			free(key); key = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/PositionMax/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "Position", sizeof("Position")))
	{
		if((ret = CPhidgetAdvancedServo_setPosition(advservo, index, value))) 
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "Engaged", sizeof("Engaged")))
	{
		int val = strtol(state, NULL, 10);
		if((ret = CPhidgetAdvancedServo_setEngaged(advservo, index, val))) 
		{
			free(key); key = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Engaged/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "SpeedRampingOn", sizeof("SpeedRampingOn")))
	{
		int val = strtol(state, NULL, 10);
		if((ret = CPhidgetAdvancedServo_setSpeedRampingOn(advservo, index, val))) 
		{
			free(key); key = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/SpeedRampingOn/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		free(key); key = NULL;
		return EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for AdvancedServo: %s\n", setThing);
	}
	free(key); key = NULL;
	return EPHIDGET_OK;
}

int phidget_encoder_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetEncoderHandle encoder = (CPhidgetEncoderHandle)phid;
	int value = strtol(state, NULL, 10);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "ResetPosition", sizeof("ResetPosition")))
	{
		CPhidgetEncoder_setPosition(encoder, index, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/ResetPosition", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for Encoder: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_gps_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetGPSHandle gps = (CPhidgetGPSHandle)phid;
	double value = strtod(state, NULL);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		ret = CPhidgetGPS_setPositionChangeTrigger(gps, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Trigger", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for GPS: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_gyroscope_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetGyroscopeHandle gyro = (CPhidgetGyroscopeHandle)phid;
	double value = strtod(state, NULL);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		ret = CPhidgetGyroscope_setAngularRateChangeTrigger(gyro, index, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Trigger", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for Gyroscope: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_interfacekit_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret;
	CPhidgetInterfaceKitHandle ifkit = (CPhidgetInterfaceKitHandle)phid;
	int value = strtol(state, NULL, 10);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Output", sizeof("Output")))
	{
		if((ret = CPhidgetInterfaceKit_setOutputState(ifkit, index, value)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "Ratiometric", sizeof("Ratiometric")))
	{
		if((ret = CPhidgetInterfaceKit_setRatiometric(ifkit, value)))
		{
			free(key); key = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Ratiometric", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		if((ret = CPhidgetInterfaceKit_setSensorChangeTrigger(ifkit, index, value)))
		{
			free(key); key = NULL;
			return ret;
		}
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Trigger/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		return EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for InterfaceKit: %s\n", setThing);
	}
	free(key); key = NULL;
	return EPHIDGET_OK;
}

int phidget_led_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetLEDHandle led = (CPhidgetLEDHandle)phid;
	int value = strtol(state, NULL, 10);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Brightness", sizeof("Brightness")))
	{
		ret = CPhidgetLED_setDiscreteLED(led, index, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Brightness/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for LED: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_motorcontrol_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetMotorControlHandle motor = (CPhidgetMotorControlHandle)phid;
	double value = strtod(state, NULL);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Acceleration", sizeof("Acceleration")))
	{
		ret = CPhidgetMotorControl_setAcceleration(motor, index, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Acceleration/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "Velocity", sizeof("Velocity")))
	{
		ret = CPhidgetMotorControl_setVelocity(motor, index, value);
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for MotorControl: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_phsensor_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetPHSensorHandle ph = (CPhidgetPHSensorHandle)phid;
	double value = strtod(state, NULL);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		ret = CPhidgetPHSensor_setPHChangeTrigger(ph, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Trigger", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	if(!strncmp(setThing, "Temperature", sizeof("Temperature")))
	{
		ret = CPhidgetPHSensor_setPHChangeTrigger(ph, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Temperature", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for PHSensor: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_rfid_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetRFIDHandle rfid = (CPhidgetRFIDHandle)phid;
	int value = strtol(state, NULL, 10);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Output", sizeof("Output")))
	{
		ret = CPhidgetRFID_setOutputState(rfid, index, value);
	}
	else if(!strncmp(setThing, "AntennaOn", sizeof("AntennaOn")))
	{
		ret = CPhidgetRFID_setAntennaOn(rfid, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/AntennaOn", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "LEDOn", sizeof("LEDOn")))
	{
		ret = CPhidgetRFID_setLEDOn(rfid, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/LEDOn", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for RFID: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_servo_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetServoHandle servo = (CPhidgetServoHandle)phid;
	double value = strtod(state, NULL);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Position", sizeof("Position")))
	{
		ret = CPhidgetServo_setPosition(servo, index, value);
	}
	else if(!strncmp(setThing, "Engaged", sizeof("Engaged")))
	{
		int val = strtol(state, NULL, 10);
		ret = CPhidgetServo_setEngaged(servo, index, val);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Engaged/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for Servo: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_stepper_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetStepperHandle stepper = (CPhidgetStepperHandle)phid;
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Acceleration", sizeof("Acceleration")))
	{
		double value = strtod(state, NULL);
		ret = CPhidgetStepper_setAcceleration(stepper, index, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Acceleration/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "VelocityLimit", sizeof("VelocityLimit")))
	{
		double value = strtod(state, NULL);
		ret = CPhidgetStepper_setVelocityLimit(stepper, index, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/VelocityLimit/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "TargetPosition", sizeof("TargetPosition")))
	{
#if defined(WINCE)
		__int64 value = (__int64)strtol(state, NULL, 10);
#elif defined(_WINDOWS)
		__int64 value = (__int64)_strtoi64(state, NULL, 10);
#else
		__int64 value = strtoll(state, NULL, 10);
#endif
		ret = CPhidgetStepper_setTargetPosition(stepper, index, value);
	}
	else if(!strncmp(setThing, "CurrentPosition", sizeof("CurrentPosition")))
	{
#if defined(WINCE)
		__int64 value = (__int64)strtol(state, NULL, 10);
#elif defined(_WINDOWS)
		__int64 value = (__int64)_strtoi64(state, NULL, 10);
#else
		__int64 value = strtoll(state, NULL, 10);
#endif
		ret = CPhidgetStepper_setCurrentPosition(stepper, index, value);
	}
	else if(!strncmp(setThing, "CurrentLimit", sizeof("CurrentLimit")))
	{
		double value = strtod(state, NULL);
		ret = CPhidgetStepper_setCurrentLimit(stepper, index, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/CurrentLimit/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "Engaged", sizeof("Engaged")))
	{
		long value = strtol(state, NULL, 10);
		ret = CPhidgetStepper_setEngaged(stepper, index, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Engaged/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for Stepper: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_temperaturesensor_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetTemperatureSensorHandle temperature = (CPhidgetTemperatureSensorHandle)phid;
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		double value = strtod(state, NULL);
		ret = CPhidgetTemperatureSensor_setTemperatureChangeTrigger(temperature, index, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Trigger/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "ThermocoupleType", sizeof("ThermocoupleType")))
	{
		double value = strtol(state, NULL, 10);
		ret = CPhidgetTemperatureSensor_setThermocoupleType(temperature, index, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/ThermocoupleType/%d", phid->deviceType, phid->serialNumber, index);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for TemperatureSensor: %s\n", setThing);
	}
	
	free(key); key = NULL;
	return ret;
}

int phidget_textlcd_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetTextLCDHandle lcd = (CPhidgetTextLCDHandle)phid;
	int value;
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Backlight", sizeof("Backlight")))
	{
		value = strtol(state, NULL, 10);
		ret = CPhidgetTextLCD_setBacklight(lcd, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Backlight", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "Contrast", sizeof("Contrast")))
	{
		value = strtol(state, NULL, 10);
		ret = CPhidgetTextLCD_setContrast(lcd, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Contrast", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "CursorOn", sizeof("CursorOn")))
	{
		value = strtol(state, NULL, 10);
		ret = CPhidgetTextLCD_setCursorOn(lcd, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/CursorOn", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "CursorBlink", sizeof("CursorBlink")))
	{
		value = strtol(state, NULL, 10);
		ret = CPhidgetTextLCD_setCursorBlink(lcd, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/CursorBlink", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "DisplayString", sizeof("DisplayString")))
	{
		ret = CPhidgetTextLCD_setDisplayString(lcd, index, (char *)state);
	}
	else if(!strncmp(setThing, "CustomCharacter", sizeof("CustomCharacter")))
	{
		int val1, val2;
		char *endptr;
		val1 = strtol(state, &endptr, 10);
		val2 = strtol(endptr+1, NULL, 10);
		ret = CPhidgetTextLCD_setCustomCharacter(lcd, index, val1, val2);
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for TextLCD: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_textled_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetTextLEDHandle textled = (CPhidgetTextLEDHandle)phid;
	int value;
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Brightness", sizeof("Brightness")))
	{
		value = strtol(state, NULL, 10);
		ret = CPhidgetTextLED_setBrightness(textled, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Brightness", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else if(!strncmp(setThing, "DisplayString", sizeof("DisplayString")))
	{
		ret = CPhidgetTextLED_setDisplayString(textled, index, (char *)state);
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for TextLED: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int phidget_weightsensor_set(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss)
{
	int ret = EPHIDGET_OK;
	CPhidgetWeightSensorHandle weight = (CPhidgetWeightSensorHandle)phid;
	double value = strtod(state, NULL);
	char *key;
	key = (char *)malloc(MAX_KEY_SIZE);
	if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		ret = CPhidgetWeightSensor_setWeightChangeTrigger(weight, value);
		snprintf(key, MAX_KEY_SIZE, "/PSK/%s/%d/Trigger", phid->deviceType, phid->serialNumber);
		if((ret = add_key(pdss, key, state)))
		{
			free(key); key = NULL;
			return ret;
		}
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		DPRINT("Bad setType for WeightSensor: %s\n", setThing);
	}
	free(key); key = NULL;
	return ret;
}

int(*fptrSet[PHIDGET_DEVICE_TYPE_COUNT])(CPhidgetHandle phid, const char *setThing, int index, const char *state, void *pdss) = {NULL, NULL, NULL,
phidget_interfacekit_set,
phidget_servo_set,
NULL, NULL,
phidget_rfid_set,
phidget_weightsensor_set,NULL,
phidget_textlcd_set,
phidget_motorcontrol_set, NULL,
phidget_led_set,
phidget_encoder_set,
phidget_stepper_set,
phidget_temperaturesensor_set,
phidget_accelerometer_set,
phidget_textled_set,
phidget_phsensor_set,
phidget_gyroscope_set,
phidget_advancedservo_set,
phidget_gps_set};

// format is: "/PCK/(devicetype)/(serial number)/(thing to set)[/(index)[/(extra info)]]"
void phidget_set(const char *k, const char *escapedv, pdict_reason_t r, const char *pde_oldval, void *pdss)
{
	regmatch_t pmatch[6];
	char *deviceType = NULL;
	char *serialNumber = NULL;
	char *setThing = NULL; //ie output
	char *index = NULL;
	char *setThingSpecific = NULL;
	char *v;
	
	int deviceID;

	long serial;
	CNetworkPhidgetInfoHandle device;

	int res, ind=-1, ret = EPHIDGET_OK;

	unescape(escapedv, &v, NULL);

	if(!strncmp(v, "\001", 1) && strlen(v) == 1)
	{
		memset(v,0,1);
	}

	if(r!=PDR_ENTRY_REMOVING)
	{
		if ((res = regexec(&phidgetsetex, k, 6, pmatch, 0)) != 0) {
			DPRINT("Error in phidget_set - pattern not met\n");
			return;
		}
		getmatchsub(k, &deviceType, pmatch, 1);
		getmatchsub(k, &serialNumber, pmatch, 2);
		getmatchsub(k, &setThing, pmatch, 3);
		getmatchsub(k, &index, pmatch, 4);
		getmatchsub(k, &setThingSpecific, pmatch, 5);// - not needed?

		//look for a device with that serial number / device type
		pthread_mutex_lock(&PhidgetsAndClientsMutex);
		if(deviceType && serialNumber)
		{
			deviceID=phidget_type_to_id(deviceType);
			serial = strtol(serialNumber, NULL, 10);
			if((findNetworkPhidgetInfoHandleInList(OpenPhidgets, serial, deviceID, &device)))
			{
				DPRINT("Couldn't find device\n");
				//TODO: send an error
				free(deviceType); deviceType = NULL;
				free(serialNumber); serialNumber = NULL;
				free(setThing); setThing = NULL;
				free(index); index = NULL;
				free(setThingSpecific); setThingSpecific = NULL;
				free(v); v = NULL;
				pthread_mutex_unlock(&PhidgetsAndClientsMutex);
				return;
			}
		}

		if(!strncmp(setThing, "Label", sizeof("Label")))
		{
			ret = CPhidget_setDeviceLabel(device->phidget, v);
		}
		else if(fptrSet[device->phidget->deviceID] && setThing)
		{
			if(index)
				ind = strtol(index, NULL, 10);
			ret = fptrSet[device->phidget->deviceID](device->phidget, setThing, ind, v, pdss);
		}
		else
			ret = EPHIDGET_INVALIDARG;//TODO: error
		
		pthread_mutex_unlock(&PhidgetsAndClientsMutex);
		free(deviceType); deviceType = NULL;
		free(serialNumber); serialNumber = NULL;
		free(setThing); setThing = NULL;
		free(index); index = NULL;
		free(setThingSpecific); setThingSpecific = NULL;
	}
	free(v); v = NULL;
	//TODO: send some sort of error response
	if(ret)
	{
		DPRINT("Error in set: %d", ret);
	}
}

// format is: "/PCK/Client/(ip address)/(client port)/(device type)/(serial number)"
void phidget_openclose(const char *k, const char *escapedv, pdict_reason_t r, const char *pde_oldval, void *pdss)
{
	regmatch_t pmatch[5];
	char *ipaddr = NULL;
	char *port = NULL;
	char *deviceType = NULL; //ie output
	char *serialNumber = NULL;
	char *v;

	long serial;
	int deviceID;

	int res;

	unescape(escapedv, &v, NULL);

	if(!strncmp(v, "\001", 1) && strlen(v) == 1)
	{
		memset(v,0,1);
	}

	if ((res = regexec(&phidgetopencloseex, k, 5, pmatch, 0)) != 0) {
		DPRINT("Error in phidget_openclose - pattern not met\n");
		return;
	}
	getmatchsub(k, &ipaddr, pmatch, 1);
	getmatchsub(k, &port, pmatch, 2);
	getmatchsub(k, &deviceType, pmatch, 3);
	getmatchsub(k, &serialNumber, pmatch, 4);

	switch(r)
	{
		case PDR_ENTRY_REMOVING:
			if(!strcmp(v, "Open")) //An open device was detached
			{
				//close devices referenced by this client
				CNetworkPhidgetListHandle trav = 0;
				CPhidgetHandle devices_to_close[128];
				int num_to_close = 0, i;
				CClientInfoHandle newClient;

				pthread_mutex_lock(&PhidgetsAndClientsMutex);
				if(!findClientInfoHandleInList(ConnectedClients, ipaddr, port, &newClient))
				{
					for (trav=newClient->phidgets; trav; trav = trav->next) {
						devices_to_close[num_to_close++] = trav->phidgetInfo->phidget;
					}
				}

				for(i=0;i<num_to_close;i++)
				{
					close_phidget(pdss, devices_to_close[i]->deviceID, devices_to_close[i]->serialNumber, ipaddr, port);
				}
				pthread_mutex_unlock(&PhidgetsAndClientsMutex);
			}
			break;
		default:
			if(deviceType && v)
			{
				if(!strcmp(v, "Open"))
				{
					if(serialNumber)
						serial = strtol(serialNumber, NULL, 10);
					else
						serial = -1;
					deviceID = phidget_type_to_id(deviceType);

					pthread_mutex_lock(&PhidgetsAndClientsMutex);
					if(open_phidget(pdss, deviceID, serial, ipaddr, port))
					{
						DPRINT("Couldn't open device\n");
						//TODO: send an error
						pthread_mutex_unlock(&PhidgetsAndClientsMutex);
						free(ipaddr); ipaddr = NULL;
						free(port); port = NULL;
						free(deviceType); deviceType = NULL;
						free(serialNumber); serialNumber = NULL;
						return;
					}
					pthread_mutex_unlock(&PhidgetsAndClientsMutex);
				}
				if(!strcmp(v, "Close") && serialNumber)
				{
					serial = strtol(serialNumber, NULL, 10);
					deviceID = phidget_type_to_id(deviceType);

					pthread_mutex_lock(&PhidgetsAndClientsMutex);
					if(close_phidget(pdss, deviceID, serial, ipaddr, port))
					{
						DPRINT("Couldn't close device\n");
						//TODO: send an error
						pthread_mutex_unlock(&PhidgetsAndClientsMutex);
						free(ipaddr); ipaddr = NULL;
						free(port); port = NULL;
						free(deviceType); deviceType = NULL;
						free(serialNumber); serialNumber = NULL;
						free(v); v = NULL;
						return;
					}
					pthread_mutex_unlock(&PhidgetsAndClientsMutex);
				}
			}
			DPRINT("Phidget %s: %s, %s, %s, %s", v, ipaddr, port, deviceType, serialNumber?serialNumber:"No Serial");
	}

	free(ipaddr); ipaddr = NULL;
	free(port); port = NULL;
	free(deviceType); deviceType = NULL;
	free(serialNumber); serialNumber = NULL;
	free(v); v = NULL;
}
