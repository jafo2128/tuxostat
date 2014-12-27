#include "stdafx.h"
#include "csocket.h"
#include "csocketevents.h"
#include "utils/utils.h"
#include "math.h"

#include "phidget21int.h"

regex_t phidgetsetex;
regex_t managerex;


static int
hexval(unsigned char c)
{
	if (isdigit(c))
		return c - '0';
	c = tolower(c);
	if (c <= 'f')
		return c - 'a' + 10;
	return 0;
}

int phidget_accelerometer_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetAccelerometerHandle phid = (CPhidgetAccelerometerHandle)generic_phid;
	if(!strncmp(setThing, "NumberOfAxes", sizeof("NumberOfAxes")))
	{
		int value = strtol(state, NULL, 10);
		phid->phid.attr.accelerometer.numAxis = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Acceleration", sizeof("Acceleration")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.accelerometer.numAxis?
			phid->phid.attr.accelerometer.numAxis:ACCEL_MAXAXES)
		{
			phid->axis[index] = value;
			FIRE(AccelerationChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "AccelerationMin", sizeof("AccelerationMin")))
	{
		double value = strtod(state, NULL);
		phid->accelerationMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "AccelerationMax", sizeof("AccelerationMax")))
	{
		double value = strtod(state, NULL);
		phid->accelerationMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.accelerometer.numAxis?
			phid->phid.attr.accelerometer.numAxis:ACCEL_MAXAXES)
		{
			phid->axisChangeTrigger[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for Accelerometer: %s", setThing);
	}
	return ret;
}

int phidget_advancedservo_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetAdvancedServoHandle phid = (CPhidgetAdvancedServoHandle)generic_phid;
	if(!strncmp(setThing, "NumberOfMotors", sizeof("NumberOfMotors")))
	{
		int value = strtol(state, NULL, 10);
		phid->phid.attr.advancedservo.numMotors = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Position", sizeof("Position")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.advancedservo.numMotors?
			phid->phid.attr.advancedservo.numMotors:ADVSERVO_MAXSERVOS)
		{
			phid->motorPositionEcho[index] = value;
			FIRE(PositionChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "PositionMax", sizeof("PositionMax")))
	{
		double value = strtod(state, NULL);
		phid->motorPositionMax[index] = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "PositionMin", sizeof("PositionMin")))
	{
		double value = strtod(state, NULL);
		phid->motorPositionMin[index] = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Acceleration", sizeof("Acceleration")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.advancedservo.numMotors?
			phid->phid.attr.advancedservo.numMotors:ADVSERVO_MAXSERVOS)
		{
			phid->motorAcceleration[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "AccelerationMax", sizeof("AccelerationMax")))
	{
		double value = strtod(state, NULL);
		phid->accelerationMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "AccelerationMin", sizeof("AccelerationMin")))
	{
		double value = strtod(state, NULL);
		phid->accelerationMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Current", sizeof("Current")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.advancedservo.numMotors?
			phid->phid.attr.advancedservo.numMotors:ADVSERVO_MAXSERVOS)
		{
			phid->motorSensedCurrent[index] = value;
			FIRE(CurrentChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "VelocityLimit", sizeof("VelocityLimit")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.advancedservo.numMotors?
			phid->phid.attr.advancedservo.numMotors:ADVSERVO_MAXSERVOS)
		{
			phid->motorVelocity[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Velocity", sizeof("Velocity")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.advancedservo.numMotors?
			phid->phid.attr.advancedservo.numMotors:ADVSERVO_MAXSERVOS)
		{
			phid->motorVelocityEcho[index] = value;
			FIRE(VelocityChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "VelocityMax", sizeof("VelocityMax")))
	{
		double value = strtod(state, NULL);
		phid->velocityMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "VelocityMin", sizeof("VelocityMin")))
	{
		double value = strtod(state, NULL);
		phid->velocityMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Engaged", sizeof("Engaged")))
	{
		int value = strtol(state, NULL, 10);
		if(index < phid->phid.attr.advancedservo.numMotors?
			phid->phid.attr.advancedservo.numMotors:ADVSERVO_MAXSERVOS)
		{
			phid->motorEngagedState[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "SpeedRampingOn", sizeof("SpeedRampingOn")))
	{
		int value = strtol(state, NULL, 10);
		if(index < phid->phid.attr.advancedservo.numMotors?
			phid->phid.attr.advancedservo.numMotors:ADVSERVO_MAXSERVOS)
		{
			phid->motorEngagedState[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Stopped", sizeof("Stopped")))
	{
		int value = strtol(state, NULL, 10);
		if(index < phid->phid.attr.advancedservo.numMotors?
			phid->phid.attr.advancedservo.numMotors:ADVSERVO_MAXSERVOS)
		{
			phid->motorStoppedState[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for Advanced Servo: %s", setThing);
	}
	return ret;
}

int phidget_encoder_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	char *endPtr;
	CPhidgetEncoderHandle phid = (CPhidgetEncoderHandle)generic_phid;
	int value = strtol(state, &endPtr, 10);
	if(!strncmp(setThing, "NumberOfEncoders", sizeof("NumberOfEncoders")))
	{
		phid->phid.attr.encoder.numEncoders = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "NumberOfInputs", sizeof("NumberOfInputs")))
	{
		phid->phid.attr.encoder.numInputs = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Input", sizeof("Input")))
	{
		if(index < phid->phid.attr.encoder.numInputs?
			phid->phid.attr.encoder.numInputs:ENCODER_MAXINPUTS)
		{
			phid->inputState[index] = value;
			FIRE(InputChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "ResetPosition", sizeof("ResetPosition")))
	{
		if(index < phid->phid.attr.encoder.numEncoders?
			phid->phid.attr.encoder.numEncoders:ENCODER_MAXENCODERS)
		{
			phid->encoderPosition[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Position", sizeof("Position")))
	{
		if(index < phid->phid.attr.encoder.numEncoders?
			phid->phid.attr.encoder.numEncoders:ENCODER_MAXENCODERS)
		{
			int tempC;
			int posnchange = strtol(endPtr+1, &endPtr, 10);
			int posn = strtol(endPtr+1, &endPtr, 10);
			
			phid->encoderPosition[index] = posn;
			
			tempC = (value - phid->encoderTimeStamp[index]);
			if (tempC > 30000) tempC = 30000;

			phid->encoderTimeStamp[index] = value;
			
			FIRE(PositionChange, index, (unsigned short)tempC, posnchange);

		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for Encoder: %s", setThing);
	}
	return ret;
}

int phidget_gps_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetGPSHandle phid = (CPhidgetGPSHandle)generic_phid;
	if(!strncmp(setThing, "Data", sizeof("Data")))
	{
		//TODO: Implement this
		//parse_GPS_data(phid, state);
	}
	else if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		double value = strtod(state, NULL);
		phid->PositionChangeTrigger = value * 0.0000899928055396;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for GPS: %s", setThing);
	}
	return ret;
}

int phidget_gyroscope_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetGyroscopeHandle phid = (CPhidgetGyroscopeHandle)generic_phid;
	if(!strncmp(setThing, "NumberOfAxes", sizeof("NumberOfAxes")))
	{
		int value = strtol(state, NULL, 10);
		phid->phid.attr.gyroscope.numAxis = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "AngularRate", sizeof("AngularRate")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.gyroscope.numAxis?
			phid->phid.attr.gyroscope.numAxis:GYRO_MAXAXES)
		{
			phid->axis[index] = value;
			FIRE(AngularRateChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.gyroscope.numAxis?
			phid->phid.attr.gyroscope.numAxis:GYRO_MAXAXES)
		{
			phid->axisChangeTrigger[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for Gyroscope: %s", setThing);
	}
	return ret;
}

int phidget_interfacekit_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetInterfaceKitHandle phid = (CPhidgetInterfaceKitHandle)generic_phid;
	int value = strtol(state, NULL, 10);
	if(!strncmp(setThing, "NumberOfSensors", sizeof("NumberOfSensors")))
	{
		phid->phid.attr.ifkit.numSensors = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "NumberOfInputs", sizeof("NumberOfInputs")))
	{
		phid->phid.attr.ifkit.numInputs = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "NumberOfOutputs", sizeof("NumberOfOutputs")))
	{
		phid->phid.attr.ifkit.numOutputs = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Input", sizeof("Input")))
	{
		if(index < phid->phid.attr.ifkit.numInputs?
			phid->phid.attr.ifkit.numInputs:IFKIT_MAXINPUTS)
		{
			phid->physicalState[index] = value;
			FIRE(InputChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Sensor", sizeof("Sensor")))
	{
		if(index < phid->phid.attr.ifkit.numSensors?
			phid->phid.attr.ifkit.numSensors:IFKIT_MAXSENSORS)
		{
			phid->sensorValue[index] = value;
			FIRE(SensorChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Output", sizeof("Output")))
	{
		if(index < phid->phid.attr.ifkit.numOutputs?
			phid->phid.attr.ifkit.numOutputs:IFKIT_MAXOUTPUTS)
		{
			phid->outputEchoStates[index] = value;
			FIRE(OutputChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		if(index < phid->phid.attr.ifkit.numSensors?
			phid->phid.attr.ifkit.numSensors:IFKIT_MAXSENSORS)
		{
			phid->sensorChangeTrigger[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Ratiometric", sizeof("Ratiometric")))
	{
		phid->ratiometric = value;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for InterfaceKit: %s", setThing);
	}
	return ret;
}

int phidget_led_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetLEDHandle phid = (CPhidgetLEDHandle)generic_phid;
	int value = strtol(state, NULL, 10);
	if(!strncmp(setThing, "NumberOfLEDs", sizeof("NumberOfLEDs")))
	{
		phid->phid.attr.led.numLEDs = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Brightness", sizeof("Brightness")))
	{
		if(index < phid->phid.attr.led.numLEDs?
			phid->phid.attr.led.numLEDs:LED_MAXLEDS)
		{
			phid->LED_Power[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for LED: %s", setThing);
	}
	return ret;
}

int phidget_motorcontrol_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetMotorControlHandle phid = (CPhidgetMotorControlHandle)generic_phid;
	if(!strncmp(setThing, "NumberOfMotors", sizeof("NumberOfMotors")))
	{
		int value = strtol(state, NULL, 10);
		phid->phid.attr.motorcontrol.numMotors = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "NumberOfInputs", sizeof("NumberOfInputs")))
	{
		int value = strtol(state, NULL, 10);
		phid->phid.attr.motorcontrol.numInputs = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Input", sizeof("Input")))
	{
		int value = strtol(state, NULL, 10);
		if(index < phid->phid.attr.motorcontrol.numInputs?
			phid->phid.attr.motorcontrol.numInputs:MOTORCONTROL_MAXINPUTS)
		{
			phid->inputState[index] = value;
			FIRE(InputChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Velocity", sizeof("Velocity")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.motorcontrol.numMotors?
			phid->phid.attr.motorcontrol.numMotors:MOTORCONTROL_MAXMOTORS)
		{
			phid->motorSpeedEcho[index] = value;
			FIRE(VelocityChange, index, value);
			//Deprecated
			FIRE(MotorChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Current", sizeof("Current")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.motorcontrol.numMotors?
			phid->phid.attr.motorcontrol.numMotors:MOTORCONTROL_MAXMOTORS)
		{
			phid->motorSensedCurrent[index] = value;
			FIRE(CurrentChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Acceleration", sizeof("Acceleration")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.motorcontrol.numMotors?
			phid->phid.attr.motorcontrol.numMotors:MOTORCONTROL_MAXMOTORS)
		{
			phid->motorAcceleration[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "AccelerationMin", sizeof("AccelerationMin")))
	{
		double value = strtod(state, NULL);
		phid->accelerationMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "AccelerationMax", sizeof("AccelerationMax")))
	{
		double value = strtod(state, NULL);
		phid->accelerationMax = value;
		phid->phid.keyCount++;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for MotorControl: %s", setThing);
	}
	return ret;
}

int phidget_phsensor_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetPHSensorHandle phid = (CPhidgetPHSensorHandle)generic_phid;
	double value = strtod(state, NULL);
	if(!strncmp(setThing, "PH", sizeof("PH")))
	{
		phid->PH = value;
		FIRE(PHChange, value);
	}
	else if(!strncmp(setThing, "PHMin", sizeof("PHMin")))
	{
		phid->phMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "PHMax", sizeof("PHMax")))
	{
		phid->phMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		phid->PHChangeTrigger = value;
	}
	else if(!strncmp(setThing, "Potential", sizeof("Potential")))
	{
		phid->Potential = value;
	}
	else if(!strncmp(setThing, "PotentialMin", sizeof("PotentialMin")))
	{
		phid->potentialMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "PotentialMax", sizeof("PotentialMax")))
	{
		phid->potentialMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Temperature", sizeof("Temperature")))
	{
		phid->Temperature = value;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for PHSensor: %s", setThing);
	}
	return ret;
}

int phidget_rfid_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetRFIDHandle phid = (CPhidgetRFIDHandle)generic_phid;
	int value = strtol(state, NULL, 10);
	if(!strncmp(setThing, "NumberOfOutputs", sizeof("NumberOfOutputs")))
	{
		phid->phid.attr.rfid.numOutputs = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Tag", sizeof("Tag")))
	{
		phid->tagPresent = 1;
		phid->lastTag[0] = (hexval(state[0])<<4)|hexval(state[1]);
		phid->lastTag[1] = (hexval(state[2])<<4)|hexval(state[3]);
		phid->lastTag[2] = (hexval(state[4])<<4)|hexval(state[5]);
		phid->lastTag[3] = (hexval(state[6])<<4)|hexval(state[7]);
		phid->lastTag[4] = (hexval(state[8])<<4)|hexval(state[9]);
		phid->lastTag[5] = 0;

		FIRE(Tag, phid->lastTag);
	}
	else if(!strncmp(setThing, "TagLoss", sizeof("TagLoss")))
	{			
		phid->tagPresent = 0;
		FIRE(TagLost, phid->lastTag);
	}
	else if(!strncmp(setThing, "TagState", sizeof("TagState")))
	{			
		phid->tagPresent = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Output", sizeof("Output")))
	{
		if(index < phid->phid.attr.rfid.numOutputs?
			phid->phid.attr.rfid.numOutputs:RFID_MAXOUTPUTS)
		{
			phid->outputEchoState[index] = value;
			FIRE(OutputChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "AntennaOn", sizeof("AntennaOn")))
	{
		phid->antennaEchoState = value;
	}
	else if(!strncmp(setThing, "LEDOn", sizeof("LEDOn")))
	{
		phid->ledEchoState = value;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for RFID: %s", setThing);
	}
	return ret;
}

int phidget_servo_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetServoHandle phid = (CPhidgetServoHandle)generic_phid;
	if(!strncmp(setThing, "NumberOfMotors", sizeof("NumberOfMotors")))
	{
		int value = strtol(state, NULL, 10);
		phid->phid.attr.led.numLEDs = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Position", sizeof("Position")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.servo.numMotors?
			phid->phid.attr.servo.numMotors:SERVO_MAXSERVOS)
		{
			phid->motorPositionEcho[index]= value;
			FIRE(PositionChange, index, value);
			//Deprecated
			FIRE(MotorPositionChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Engaged", sizeof("Engaged")))
	{
		int value = strtol(state, NULL, 10);
		if(index < phid->phid.attr.servo.numMotors?
			phid->phid.attr.servo.numMotors:SERVO_MAXSERVOS)
		{
			phid->motorEngagedState[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "PositionMin", sizeof("PositionMin")))
	{
		double value = strtod(state, NULL);
		phid->motorPositionMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "PositionMax", sizeof("PositionMax")))
	{
		double value = strtod(state, NULL);
		phid->motorPositionMax = value;
		phid->phid.keyCount++;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for Servo: %s", setThing);
	}
	return ret;
}

int phidget_stepper_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetStepperHandle phid = (CPhidgetStepperHandle)generic_phid;
	if(!strncmp(setThing, "NumberOfMotors", sizeof("NumberOfMotors")))
	{
		int value = strtol(state, NULL, 10);
		phid->phid.attr.stepper.numMotors = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "NumberOfInputs", sizeof("NumberOfInputs")))
	{
		int value = strtol(state, NULL, 10);
		phid->phid.attr.stepper.numInputs = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Input", sizeof("Input")))
	{
		int value = strtol(state, NULL, 10);
		if(index < phid->phid.attr.stepper.numInputs?
			phid->phid.attr.stepper.numInputs:STEPPER_MAXINPUTS)
		{
			phid->inputState[index] = value;
			FIRE(InputChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
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
		if(index < phid->phid.attr.stepper.numMotors?
			phid->phid.attr.stepper.numMotors:STEPPER_MAXSTEPPERS)
		{
			phid->motorPositionEcho[index] = value;
			FIRE(PositionChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
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
		if(index < phid->phid.attr.stepper.numMotors?
			phid->phid.attr.stepper.numMotors:STEPPER_MAXSTEPPERS)
		{
			phid->motorPosition[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "PositionMin", sizeof("PositionMin")))
	{
#if defined(WINCE)
		__int64 value = (__int64)strtol(state, NULL, 10);
#elif defined(_WINDOWS)
		__int64 value = (__int64)_strtoi64(state, NULL, 10);
#else
		__int64 value = strtoll(state, NULL, 10);
#endif
		phid->motorPositionMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "PositionMax", sizeof("PositionMax")))
	{
#if defined(WINCE)
		__int64 value = (__int64)strtol(state, NULL, 10);
#elif defined(_WINDOWS)
		__int64 value = (__int64)_strtoi64(state, NULL, 10);
#else
		__int64 value = strtoll(state, NULL, 10);
#endif
		phid->motorPositionMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Acceleration", sizeof("Acceleration")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.stepper.numMotors?
			phid->phid.attr.stepper.numMotors:STEPPER_MAXSTEPPERS)
		{
			phid->motorAcceleration[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "AccelerationMin", sizeof("AccelerationMin")))
	{
		double value = strtod(state, NULL);
		phid->accelerationMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "AccelerationMax", sizeof("AccelerationMax")))
	{
		double value = strtod(state, NULL);
		phid->accelerationMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "CurrentLimit", sizeof("CurrentLimit")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.stepper.numMotors?
			phid->phid.attr.stepper.numMotors:STEPPER_MAXSTEPPERS)
		{
			phid->motorCurrentLimit[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Current", sizeof("Current")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.stepper.numMotors?
			phid->phid.attr.stepper.numMotors:STEPPER_MAXSTEPPERS)
		{
			phid->motorSensedCurrent[index] = value;
			FIRE(CurrentChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "CurrentMin", sizeof("CurrentMin")))
	{
		double value = strtod(state, NULL);
		phid->currentMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "CurrentMax", sizeof("CurrentMax")))
	{
		double value = strtod(state, NULL);
		phid->currentMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "VelocityLimit", sizeof("VelocityLimit")))
	{
		double value = strtod(state, NULL);
		phid->motorSpeed[index] = value;
	}
	else if(!strncmp(setThing, "Velocity", sizeof("Velocity")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.stepper.numMotors?
			phid->phid.attr.stepper.numMotors:STEPPER_MAXSTEPPERS)
		{
			phid->motorSpeedEcho[index] = value;
			FIRE(VelocityChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "VelocityMin", sizeof("VelocityMin")))
	{
		double value = strtod(state, NULL);
		phid->motorSpeedMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "VelocityMax", sizeof("VelocityMax")))
	{
		double value = strtod(state, NULL);
		phid->motorSpeedMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Engaged", sizeof("Engaged")))
	{
		int value = strtol(state, NULL, 10);
		if(index < phid->phid.attr.stepper.numMotors?
			phid->phid.attr.stepper.numMotors:STEPPER_MAXSTEPPERS)
		{
			phid->motorEngagedStateEcho[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "Stopped", sizeof("Stopped")))
	{
		int value = strtol(state, NULL, 10);
		if(index < phid->phid.attr.stepper.numMotors?
			phid->phid.attr.stepper.numMotors:STEPPER_MAXSTEPPERS)
		{
			phid->motorStoppedState[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for Stepper: %s", setThing);
	}
	return ret;
}

int phidget_temperaturesensor_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetTemperatureSensorHandle phid = (CPhidgetTemperatureSensorHandle)generic_phid;
	if(!strncmp(setThing, "NumberOfSensors", sizeof("NumberOfSensors")))
	{
		int value = strtol(state, NULL, 10);
		phid->phid.attr.temperaturesensor.numTempInputs = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Potential", sizeof("Potential")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.temperaturesensor.numTempInputs?
			phid->phid.attr.temperaturesensor.numTempInputs:TEMPSENSOR_MAXSENSORS)
		{
			phid->Potential[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "PotentialMin", sizeof("PotentialMin")))
	{
		double value = strtod(state, NULL);
		phid->potentialMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "PotentialMax", sizeof("PotentialMax")))
	{
		double value = strtod(state, NULL);
		phid->potentialMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Temperature", sizeof("Temperature")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.temperaturesensor.numTempInputs?
			phid->phid.attr.temperaturesensor.numTempInputs:TEMPSENSOR_MAXSENSORS)
		{
			phid->Temperature[index] = value;
			FIRE(TemperatureChange, index, value);
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else if(!strncmp(setThing, "TemperatureMin", sizeof("TemperatureMin")))
	{
		double value = strtod(state, NULL);
		phid->temperatureMin[index] = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "TemperatureMax", sizeof("TemperatureMax")))
	{
		double value = strtod(state, NULL);
		phid->temperatureMax[index] = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "AmbientTemperature", sizeof("AmbientTemperature")))
	{
		double value = strtod(state, NULL);
		phid->AmbientTemperature = value;
	}
	else if(!strncmp(setThing, "AmbientTemperatureMin", sizeof("AmbientTemperatureMin")))
	{
		double value = strtod(state, NULL);
		phid->ambientTemperatureMin = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "AmbientTemperatureMax", sizeof("AmbientTemperatureMax")))
	{
		double value = strtod(state, NULL);
		phid->ambientTemperatureMax = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "ThermocoupleType", sizeof("ThermocoupleType")))
	{
		int value = strtol(state, NULL, 10);
		phid->ThermocoupleType[index] = value;
	}
	else if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		double value = strtod(state, NULL);
		if(index < phid->phid.attr.temperaturesensor.numTempInputs?
			phid->phid.attr.temperaturesensor.numTempInputs:TEMPSENSOR_MAXSENSORS)
		{
			phid->TempChangeTrigger[index] = value;
		}
		else
			ret = EPHIDGET_OUTOFBOUNDS;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for TemperatureSensor: %s", setThing);
	}
	return ret;
}

int phidget_textlcd_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetTextLCDHandle phid = (CPhidgetTextLCDHandle)generic_phid;
	int value = strtol(state, NULL, 10);
	if(!strncmp(setThing, "NumberOfRows", sizeof("NumberOfRows")))
	{
		phid->phid.attr.textlcd.numRows = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "NumberOfColumns", sizeof("NumberOfColumns")))
	{
		phid->phid.attr.textlcd.numColumns = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Backlight", sizeof("Backlight")))
	{
		phid->m_blnBacklight = (unsigned char)value;
	}
	else if(!strncmp(setThing, "CursorOn", sizeof("CursorOn")))
	{
		phid->m_blnCursorOn = (unsigned char)value;
	}
	else if(!strncmp(setThing, "CursorBlink", sizeof("CursorBlink")))
	{
		phid->m_blnCursorBlink = (unsigned char)value;
	}
	else if(!strncmp(setThing, "Contrast", sizeof("Contrast")))
	{
		phid->m_iContrast = (unsigned char)value;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for TextLCD: %s", setThing);
	}
	return ret;
}

int phidget_textled_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetTextLEDHandle phid = (CPhidgetTextLEDHandle)generic_phid;
	int value = strtol(state, NULL, 10);
	if(!strncmp(setThing, "NumberOfRows", sizeof("NumberOfRows")))
	{
		phid->phid.attr.textled.numRows = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "NumberOfColumns", sizeof("NumberOfColumns")))
	{
		phid->phid.attr.textled.numColumns = value;
		phid->phid.keyCount++;
	}
	else if(!strncmp(setThing, "Brightness", sizeof("Brightness")))
	{
		phid->brightness = value;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for TextLED: %s", setThing);
	}
	return ret;
}

int phidget_weightsensor_set(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state)
{
	int ret = EPHIDGET_OK;
	CPhidgetWeightSensorHandle phid = (CPhidgetWeightSensorHandle)generic_phid;
	double value = strtod(state, NULL);
	if(!strncmp(setThing, "Weight", sizeof("Weight")))
	{
		phid->Weight = value;
		FIRE(WeightChange, value);
	}
	else if(!strncmp(setThing, "Trigger", sizeof("Trigger")))
	{
		phid->WeightChangeTrigger = value;
	}
	else{
		ret = EPHIDGET_INVALIDARG;
		LOG(PHIDGET_LOG_DEBUG,"Bad setType for WeightSensor: %s", setThing);
	}
	return ret;
}

int(*fptrSet[PHIDGET_DEVICE_TYPE_COUNT])(CPhidgetHandle generic_phid, const char *setThing, int index, const char *state) = {NULL, NULL, NULL,
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

//Number of initialization keys required - these are just the device specific ones
int keysNeeded[PHIDGET_DEVICE_TYPE_COUNT] = {0, 0, 0,
3 /*phidget_interfacekit*/,
3 /*phidget_servo*/,
0, 0,
2 /*phidget_rfid*/,
0 /*phidget_weightsensor*/,0,
2 /*phidget_textlcd*/,
4 /*phidget_motorcontrol*/, 0,
1 /*phidget_led*/,
2 /*phidget_encoder*/,
10 /*phidget_stepper*/,
7 /*phidget_temperaturesensor*/,
3 /*phidget_accelerometer*/,
2 /*phidget_textled*/,
4 /*phidget_phsensor*/,
1 /*phidget_gyroscope*/,
7 /*phidget_advancedservo*/,
0 /*phidget_gps*/};

void
network_phidget_event_handler(const char *key, const char *val, unsigned int len, pdict_reason_t reason, void *ptr)
{
	CPhidgetHandle phid = (CPhidgetHandle)ptr;
	regmatch_t pmatch[6];
	char *setThing = NULL;
	char *index = NULL;
	char *serial = NULL;
	char errbuf[1024];
	
	int serialNumber;

	int res, ind=-1, version, i, ret = EPHIDGET_OK;

	if(!strncmp(val, "\001", 1) && len == 1)
	{
		memset((char *)val,0,1);
	}

	if(reason!=PDR_ENTRY_REMOVING || !strncmp(val, "Detached", sizeof("Detached")))
	{
		if ((res = regexec(&phidgetsetex, key, 6, pmatch, 0)) != 0) {
			LOG(PHIDGET_LOG_DEBUG,"Error in network_phidget_event_handler - pattern not met");
			return;
		}
		getmatchsub(key, &serial, pmatch, 2);
		getmatchsub(key, &setThing, pmatch, 3);
		getmatchsub(key, &index, pmatch, 4);

		serialNumber = strtol(serial, NULL, 10);
			
		if(phid->specificDevice == 0 && strncmp(val, "Detached", sizeof("Detached")))
		{
			phid->specificDevice = 2;
			phid->serialNumber = serialNumber;
		}
		
		if(serialNumber == phid->serialNumber && setThing)
		{

			if(!strncmp(setThing, "Label", sizeof("Label")))
			{
				strncpy(phid->label, val, 11);
				phid->keyCount++;
			}
			else if(!strncmp(setThing, "Version", sizeof("Version")))
			{
				version = strtol(val, NULL, 10);
				phid->deviceVersion = version;
				phid->keyCount++;
			}
			else if(!strncmp(setThing, "Name", sizeof("Name")))
			{
				for(i = 0;i<PHIDGET_DEVICE_COUNT;i++)
				{
					if(!strcmp(val, Phid_DeviceSpecificName[i]))
					{
						phid->deviceIDSpec = i;
						phid->attr = Phid_Device_Def[i].pdd_attr;
						phid->keyCount++;
						break;
					}
				}

			}
			else if(!strncmp(setThing, "Status", sizeof("Status")))
			{
				if(!strncmp(val, "Attached", sizeof("Attached")))
				{
					phid->deviceType = Phid_DeviceName[phid->deviceID];
					phid->keyCount++;
					
				}
				else if(!strncmp(val, "Detached", sizeof("Detached")))
				{
					CThread_mutex_lock(&phid->lock);
					phid->keyCount = 0;
					if(phid->specificDevice == 2)
					{
						phid->specificDevice = 0;
					}
					phid->status &= ~PHIDGET_ATTACHED_FLAG;
					phid->status |= PHIDGET_DETACHING_FLAG;
					CThread_mutex_unlock(&phid->lock);

					if (phid->fptrDetach)
						phid->fptrDetach((CPhidgetHandle)phid, phid->fptrDetachptr);

					//clear all variables
					phid->fptrClear((CPhidgetHandle)phid);
					
					//if mDNS & any server, disconnect
					if(!phid->networkInfo->requested_address && !phid->networkInfo->requested_serverID)
					{
						CThread DisconnectPhidgetThread;
						CThread_mutex_lock(&zeroconfPhidgetsLock);
						CList_removeFromList((CListHandle *)&zeroconfPhidgets, phid, CPhidget_areExtraEqual, TRUE, CPhidget_free);
						CThread_mutex_unlock(&zeroconfPhidgetsLock);
						CThread_create(&DisconnectPhidgetThread, DisconnectPhidgetThreadFunction, phid);
					}

					phid->status &= ~PHIDGET_DETACHING_FLAG;
						
					phid->deviceIDSpec = 0;
					ZEROMEM(&phid->attr, sizeof(CPhidgetAttr));
					ZEROMEM(phid->label, 11);
					phid->deviceVersion = 0;

				}
				else
				{
					throw_error_event(phid, "Bad Message type for Status set", EPHIDGET_NETWORK);
				}
			}
			else if(fptrSet[phid->deviceID] && setThing)
			{
				if(index)
					ind = strtol(index, NULL, 10);
				ret = fptrSet[phid->deviceID](phid, setThing, ind, val);
			}
			
			if((phid->keyCount >= (4 + keysNeeded[phid->deviceID])) && !CPhidget_statusFlagIsSet(phid, PHIDGET_ATTACHED_FLAG))
			{
				CPhidget_setStatusFlag(phid, PHIDGET_ATTACHED_FLAG);
				if (phid->fptrAttach)
					phid->fptrAttach(phid, phid->fptrAttachptr);
				phid->fptrEvents((CPhidgetHandle)phid);
			}
		}

		free(setThing); setThing = NULL;
		free(index); index = NULL;
		free(serial); serial = NULL;
	}
	
	if(ret)
	{
		snprintf(errbuf, 1024, "Problem during Network set: %s", CPhidget_strerror(ret));
		throw_error_event(phid, errbuf, ret);
	}
}

void
network_manager_event_handler(const char *key, const char *val, unsigned int vallen, pdict_reason_t reason, void *ptr)
{
	CPhidgetManagerHandle phidm = (CPhidgetManagerHandle)ptr;
	regmatch_t pmatch[6];
	char *deviceType = NULL;
	char *serial = NULL;
	char *version = NULL;
	char *deviceIDSpec = NULL;
	char *label = NULL;
	char errbuf[1024];
	
	int serialNumber;
	CPhidgetHandle phid;

	//BL:Changed to init Len before use
	int len = 0;

	int res, ret = EPHIDGET_OK;

	int i;
	
	if(!phidm) return;

	if(!strncmp(val, "\001", 1) && (len == 1))
	{
		memset((char *)val,0,1);
	}

	if(reason!=PDR_ENTRY_REMOVING)
	{
		if ((res = regexec(&managerex, key, 6, pmatch, 0)) != 0) {
			LOG(PHIDGET_LOG_DEBUG,"Error in network_manager_event_handler - pattern not met");
			return;
		}
		getmatchsub(key, &deviceType, pmatch, 1);
		getmatchsub(key, &serial, pmatch, 2);
		getmatchsub(key, &version, pmatch, 4);
		getmatchsub(key, &deviceIDSpec, pmatch, 3);
		//getmatchsub(key, &label, pmatch, 5);
		
		serialNumber = strtol(serial, NULL, 10);
	
		if((CPhidget_create(&phid))) return;

		phid->deviceID = phidget_type_to_id(deviceType);
		phid->deviceType = Phid_DeviceName[phid->deviceID];
		phid->serialNumber = serialNumber;
		phid->deviceIDSpec = (unsigned short)strtol(deviceIDSpec, NULL, 10);
		phid->deviceVersion = strtol(version, NULL, 10);
		if(label)
		{
			len = (int)strlen(label);
			if(len>10) len = 10;
			for(i=0;i<len;i++)
			{
				phid->label[i] = label[i];
			}
			phid->label[i] = '\0';
		}

		if(!strcmp(val, "Attached"))
		{
			CPhidget_setStatusFlag(phid, PHIDGET_ATTACHED_FLAG);
		
			CList_addToList((CListHandle *)phidm->AttachedPhidgets, phid, CPhidget_areEqual);

			if (phidm->fptrAttachChange && phidm->state == PHIDGETMANAGER_ACTIVE)
				phidm->fptrAttachChange((CPhidgetHandle)phid, phidm->fptrAttachChangeptr);
		}
		
		if(!strcmp(val, "Detached"))
		{
			CPhidget_clearStatusFlag(phid, PHIDGET_ATTACHED_FLAG);
			CPhidget_setStatusFlag(phid, PHIDGET_DETACHING_FLAG);
			if(CList_findInList((CListHandle)phidm->AttachedPhidgets, phid, CPhidget_areEqual, NULL))
			{
				CList_removeFromList((CListHandle *)phidm->AttachedPhidgets, phid, CPhidget_areEqual, PTRUE, CPhidget_free);
			
				if (phidm->fptrDetachChange && phidm->state == PHIDGETMANAGER_ACTIVE)
					phidm->fptrDetachChange((CPhidgetHandle)phid, phidm->fptrDetachChangeptr);
			}
			CPhidget_clearStatusFlag(phid, PHIDGET_DETACHING_FLAG);
			CPhidget_free(phid); phid = NULL;
		}

		free(deviceType); deviceType = NULL;
		//free(label); label = NULL;
		free(serial); serial = NULL;
		free(version); version = NULL;
		free(deviceIDSpec); deviceIDSpec = NULL;
	}
	
	if(ret)
	{
		snprintf(errbuf, 1024, "Problem during Network set: %s", CPhidget_strerror(ret));
		throw_error_event((CPhidgetHandle)phidm, errbuf, ret);
	}
}
