#ifndef __CPHIDGETATTR
#define __CPHIDGETATTR

typedef struct {
	int numAxis;
} CPhidgetAccelerometerAttr;

typedef struct {
	int numMotors;
} CPhidgetAdvancedServoAttr;

typedef struct {
	int numEncoders;
	int numInputs;
} CPhidgetEncoderAttr;

typedef struct {
	int dummy;
} CPhidgetGPSAttr;

typedef struct {
	int numAxis;
} CPhidgetGyroscopeAttr;

typedef struct {
	int numSensors;
	int numInputs;
	int numOutputs;
} CPhidgetInterfaceKitAttr;

typedef struct {
	int numLEDs;
} CPhidgetLEDAttr;

typedef struct {
	int numMotors;
	int numInputs;
} CPhidgetMotorControlAttr;

typedef struct {
	int dummy;
} CPhidgetPHSensorAttr;

typedef struct {
	int numOutputs;
} CPhidgetRFIDAttr;

typedef struct {
	int numMotors;
} CPhidgetServoAttr;

typedef struct {
	int numMotors;
	int numInputs;
} CPhidgetStepperAttr;

typedef struct {
	int numTempInputs;
} CPhidgetTemperatureSensorAttr;

typedef struct {
	int numTempInputs;
} CPhidgetHumiditySensorAttr;

typedef struct {
	int numRows;
	int numColumns;
} CPhidgetTextLCDAttr;

typedef struct {
	int numRows;
	int numColumns;
} CPhidgetTextLEDAttr;

typedef struct {
	int dummy;
} CPhidgetWeightSensorAttr;

typedef struct {
	int dummy1;
	int dummy2;
	int dummy3;
	int dummy4;
} CPhidgetDummyAttr;

typedef union {
#ifdef _MSC_EXTENSIONS
	CPhidgetDummyAttr dummy; /* field for unnamed initializers */
#endif
	CPhidgetAccelerometerAttr accelerometer;
	CPhidgetAdvancedServoAttr advancedservo;
	CPhidgetEncoderAttr encoder;
	CPhidgetGPSAttr gps;
	CPhidgetGyroscopeAttr gyroscope;
	CPhidgetInterfaceKitAttr ifkit;
	CPhidgetLEDAttr led;
	CPhidgetMotorControlAttr motorcontrol;
	CPhidgetPHSensorAttr phsensor;
	CPhidgetRFIDAttr rfid;
	CPhidgetServoAttr servo;
	CPhidgetStepperAttr stepper;
	CPhidgetTemperatureSensorAttr temperaturesensor;
	CPhidgetTextLCDAttr textlcd;
	CPhidgetTextLEDAttr textled;
	CPhidgetWeightSensorAttr weightsensor;
	CPhidgetHumiditySensorAttr humiditysensor;
} CPhidgetAttr;

#endif

