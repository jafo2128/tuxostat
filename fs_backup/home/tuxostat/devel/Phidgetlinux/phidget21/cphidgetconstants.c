#include "stdafx.h"
#include "cphidgetconstantsinternal.h"
#include "cphidgetconstants.h"

const char *Phid_ErrorDescriptions[PHIDGET_ERROR_CODE_COUNT] = {
"Function completed successfully.",
"A Phidget matching the type and or serial number could not be found.", 
"Memory could not be allocated.",
"Unexpected Error.  Contact Phidgets Inc. for support.", 
"Invalid argument passed to function.",
"Phidget not physically attached.", 
"Read/Write operation was interrupted.", 
"The Error Code is not defined.",
"Network Error.",
"Value is Unknown (State not yet received from device, or not yet set by user).",
"Authorization Failed.",
"Not Supported.",
"Duplicated request.",
"Given timeout has been exceeded.",
"Index out of Bounds.",
"A non-null error code was returned from an event handler.",
"A connection to the server does not exist.",
"Function is not applicable for this device."};

const char Phid_UnknownErrorDescription[] = "Unknown Error Code.";

const char *Phid_DeviceName[PHIDGET_DEVICE_TYPE_COUNT] = {"", "", "", 
"PhidgetInterfaceKit", 
"PhidgetServo", "", "", 
"PhidgetRFID", 
"PhidgetWeightSensor", "",
"PhidgetTextLCD",
"PhidgetMotorControl", "", 
"PhidgetLED",
"PhidgetEncoder",
"PhidgetStepper",
"PhidgetTemperatureSensor",
"PhidgetAccelerometer",
"PhidgetTextLED",
"PhidgetPHSensor",
"PhidgetGyroscope",
"PhidgetAdvancedServo",
"PhidgetGPS"};

const char *Phid_DeviceSpecificName[PHIDGET_DEVICE_COUNT] = {
"Uninitialized Phidget Handle",
"Phidget Servo Controller 1-motor",
"Phidget InterfaceKit 8/8/8",
"Phidget InterfaceKit 0/0/4",
"Phidget InterfaceKit 0/16/16",
"Phidget InterfaceKit 0/5/7",
"Phidget InterfaceKit 0/8/8",

"Phidget Accelerometer 2-axis",

"Phidget TextLCD Custom",
"Phidget TextLCD",
"Phidget TextLCD",

"Phidget RFID",

"Phidget LED 64",
"Phidget TextLED 1x8",
"Phidget Low Voltage Motor Controller 2-motor 4-input",
"Phidget Temperature Sensor",
"Phidget Stepper",
"Phidget Encoder 1-encoder 1-input",

/* Obsoleted */
"Phidget InterfaceKit 4/8/8",
"Phidget Servo Controller 1-motor",
"Phidget Servo Controller 4-motor",
/* Not obsoleted */
"Phidget Weight Sensor",
"Phidget Humidity Sensor",

"Phidget PH Sensor",
"Phidget Gyroscope",
"Phidget Advanced Servo Controller 8-motor",
"Phidget RFID 4-output",
"Phidget TextLED 4x8",
"Phidget Touch Slider", //linear touch sensor
"Phidget Touch Rotation", //rotary touch sensor
"Phidget Encoder 4-encoder 4-input",
"Phidget GPS",
"Phidget High Speed Encoder 1-encoder",
"Phidget Unipolar Stepper Controller 4-motor",
"Phidget Bipolar Stepper Controller 1-motor",
"Phidget Joystick Interface Kit",
"Phidget TextLCD",
"Phidget InterfaceKit 8/8/8",
"Phidget Accelerometer 3-axis",
"Phidget Accelerometer 3-axis",
"Phidget Gyroscope 3-axis",
"Phidget Servo Controller 4-motor",
"Phidget High Current Motor Controller 2-motor",
"Phidget InterfaceKit 0/0/8",
"Phidget Advanced Servo Controller 1-motor"};

#ifdef DEBUG
const char LibraryVersion[] = "Phidget21 Debug - Version 2.1.4 - Built " __DATE__
    " " __TIME__;
#else
const char LibraryVersion[] = "Phidget21 - Version 2.1.4 - Built " __DATE__
    " " __TIME__;
#endif

const char Phid_Device_Def_WorkAroundA[PHIDGET_DEVICE_COUNT][2] = {
{0,0},		// PHIDSPEC_NOTHING					0x00
{0,0},		// PHIDSPEC_SERVO_1MOTOR			0x01
{0,0},		// PHIDSPEC_INTERFACEKIT_8_8_8		0x02
{0,0},		// PHIDSPEC_INTERFACEKIT_0_0_4		0x03
{0,0},		// PHIDSPEC_INTERFACEKIT_0_16_16	0x04
{0,0},		// PHIDSPEC_INTERFACEKIT_0_5_7		0x05
{0,0},		// PHIDSPEC_INTERFACEKIT_0_8_8		0x06
{0,0},		// PHIDSPEC_ACCELEROMETER			0x07
{0,0},		// PHIDSPEC_TEXTLCD_2_20			0x08
{0,0},		// PHIDSPEC_TEXTLCD_2_20_CUSTOM		0x09
{0,0},		// PHIDSPEC_TEXTLCD_2_20_COMP1		0x0A
{1,103},	// PHIDSPEC_RFID					0x0B	- I don't think it's needed at this version, but I'll leave it anyways
{0,0},		// PHIDSPEC_LED						0x0C
{0,0},		// PHIDSPEC_TEXTLED					0x0D
{0,0},		// PHIDSPEC_MOTORCONTROL			0x0E
{0,0},		// PHIDSPEC_TEMPERATURESENSOR		0x0F
{0,0},		// PHIDSPEC_STEPPER					0x10
{1,100},	// PHIDSPEC_ENCODER					0x11
{1,100},	// PHIDSPEC_INTERFACEKIT_4_8_8		0x12
{1,0},		// PHIDSPEC_SERVO_1MOTOR_OLD		0x13
{1,0},		// PHIDSPEC_SERVO_4MOTOR_OLD		0x14
{0,0},		// PHIDSPEC_WEIGHTSENSOR			0x15
{0,0},		// PHIDSPEC_HUMIDITYSENSOR			0x16
{0,0},		// PHIDSPEC_PHSENSOR				0x17
{0,0},		// PHIDSPEC_GYROSCOPE				0x18
{0,0},		// PHIDSPEC_ADVANCEDSERVO8			0x19
{0,0},		// PHIDSPEC_RFIDB					0x1A
{0,0},		// PHIDSPEC_TEXTLED2				0x1B
{0,0},		// PHIDSPEC_LINEAR_SENSOR			0x1C
{0,0},		// PHIDSPEC_ROTARY_SENSOR			0x1D
{0,0},		// PHIDSPEC_ENCODER_4				0x1E
{0,0},		// PHIDSPEC_GPS						0x1F
{0,0},		// PHIDSPEC_ENCODER_HS				0x20
{0,0},		// PHIDSPEC_UNIPOLAR_STEPPER		0x21
{0,0},		// PHIDSPEC_BIPOLAR_STEPPER			0x22
{0,0},		// PHIDSPEC_JOYSTICK				0x23
{0,0},		// PHIDSPEC_TEXTLCD_with_888		0x24
{0,0},		// PHIDSPEC_888_with_lcd			0x25
{0,0},		// PHIDSPEC_ACCELEROMETER3			0x26
{0,0},		// PHIDSPEC_ACCELEROMETER_w_GYRO	0x27
{0,0},		// PHIDSPEC_GYRO_w_ACCELEROMETER	0x28
{0,0},		// PHIDSPEC_SERVO_4MOTOR			0x29
{0,0},		// PHIDSPEC_MOTORCONTROL_HC			0x2A
{0,0},		// PHIDSPEC_INTERFACEKIT_0_0_8		0x2B
{0,0}		// PHIDSPEC_ADVANCEDSERVO1			0x2C
};

#ifndef _MSC_EXTENSIONS
#define UFINTS(name, initializer...) .name = { initializer }
#else
#define UFINTS(name, ...) { __VA_ARGS__ }
#endif

// We own Product IDs 0x30 - 0xAF
const CPhidgetDeviceDef Phid_Device_Def[] = {
{ PHIDDEF_NOTHING_01,			0x000, 0x00, { UFINTS(servo,				0) },			0 },
{ PHIDDEF_SERVO,				0x6C2, 0x39, { UFINTS(servo,				1) },			0 },

{ PHIDDEF_INTERFACEKIT,			0x6C2, 0x45, { UFINTS(ifkit,				8, 8, 8) },		0 },
{ PHIDDEF_INTERFACEKIT,			0x6C2, 0x40, { UFINTS(ifkit,				0, 0, 4) },		0 },
{ PHIDDEF_INTERFACEKIT,			0x6C2, 0x44, { UFINTS(ifkit,				0, 16, 16) },	0 },
{ PHIDDEF_INTERFACEKIT,			0x6C2, 0x51, { UFINTS(ifkit,				0, 5, 7) },		0 },
{ PHIDDEF_INTERFACEKIT,			0x6C2, 0x53, { UFINTS(ifkit,				0, 8, 8) },		0 },

{ PHIDDEF_ACCELEROMETER,		0x6C2, 0x71, { UFINTS(accelerometer,		2) },			0 },

{ PHIDDEF_TEXTLCD,				0x6C2, 0x51, { UFINTS(textlcd,				2, 20) },		0 },
{ PHIDDEF_TEXTLCD,				0x6C2, 0x52, { UFINTS(textlcd,				2, 20) },		0 },
{ PHIDDEF_TEXTLCD,				0x6C2, 0x53, { UFINTS(textlcd,				2, 20) },		0 },

{ PHIDDEF_RFID,					0x6C2, 0x30, { UFINTS(rfid,					0) },			0 },

{ PHIDDEF_LED,					0x6C2, 0x4A, { UFINTS(led,					64) },			0 },
{ PHIDDEF_TEXTLED,				0x6C2, 0x49, { UFINTS(textled,				1, 8) },		0 },
{ PHIDDEF_MOTORCONTROL,			0x6C2, 0x58, { UFINTS(motorcontrol,			2, 4) },		0 },
{ PHIDDEF_TEMPERATURESENSOR,	0x6C2, 0x70, { UFINTS(temperaturesensor,	1) },			0 },
{ PHIDDEF_STEPPER,				0x6C2, 0x47, { UFINTS(stepper,				4, 2) },		0 },
{ PHIDDEF_ENCODER,				0x6C2, 0x4B, { UFINTS(encoder,				1, 1) },		0 },

/* Obsoleted */
{ PHIDDEF_INTERFACEKIT,			0x925, 0x8201, { UFINTS(ifkit,				4, 8, 8) },		0 },
{ PHIDDEF_SERVO,				0x925, 0x8101, { UFINTS(servo,				1) },			0 },
{ PHIDDEF_SERVO,				0x925, 0x8104, { UFINTS(servo,				4) },			0 },

/* Not obsoleted */
{ PHIDDEF_WEIGHTSENSOR,			0x6C2, 0x72, { UFINTS(weightsensor,			0) },			0 },
{ PHIDDEF_HUMIDITYSENSOR,		0x6C2, 0x73, { UFINTS(humiditysensor,		0) },			0 },
{ PHIDDEF_PHSENSOR,				0x6C2, 0x74, { UFINTS(phsensor,				0) },			0 },
{ PHIDDEF_GYROSCOPE,			0x6C2, 0x75, { UFINTS(gyroscope,			1) },			0 },
{ PHIDDEF_ADVANCEDSERVO,		0x6C2, 0x3A, { UFINTS(advancedservo,		8) },			0 },
{ PHIDDEF_RFID,					0x6C2, 0x31, { UFINTS(rfid,					2) },			0 },
{ PHIDDEF_TEXTLED,				0x6C2, 0x48, { UFINTS(textled,				4, 8) },		0 },
{ PHIDDEF_INTERFACEKIT,			0x6C2, 0x76, { UFINTS(ifkit,				1, 2) },		0 }, //linear touch sensor
{ PHIDDEF_INTERFACEKIT,			0x6C2, 0x77, { UFINTS(ifkit,				1, 2) },		0 }, //rotary touch sensor
{ PHIDDEF_ENCODER,				0x6C2, 0x78, { UFINTS(encoder,				4, 4) },		0 }, //4 encoder
{ PHIDDEF_GPS,					0x6C2, 0x79, { UFINTS(gps,					1) },			0 }, 
{ PHIDDEF_ENCODER,				0x6C2, 0x80, { UFINTS(encoder,				1) },			0 }, //high speed encoder
{ PHIDDEF_STEPPER,				0x6C2, 0x7A, { UFINTS(stepper,				4) },			0 }, //unipolar stepper
{ PHIDDEF_STEPPER,				0x6C2, 0x7B, { UFINTS(stepper,				1, 4) },		0 }, //bipolar
{ PHIDDEF_INTERFACEKIT,			0x6C2, 0x7C, { UFINTS(ifkit,				2, 1) },		0 }, //joystick
{ PHIDDEF_TEXTLCD,				0x6C2, 0x7D, { UFINTS(textlcd,				2, 20) },		1 },
{ PHIDDEF_INTERFACEKIT,			0x6C2, 0x7D, { UFINTS(ifkit,				8, 8, 8) },		0 },
{ PHIDDEF_ACCELEROMETER,		0x6C2, 0x7E, { UFINTS(accelerometer,		3) },			0 }, //3-axis accelerometer
{ PHIDDEF_ACCELEROMETER,		0x6C2, 0x7F, { UFINTS(accelerometer,		3) },			1 }, //AccelGyro composite device
{ PHIDDEF_GYROSCOPE,			0x6C2, 0x7F, { UFINTS(gyroscope,			3) },			0 }, //AccelGyro composite device
{ PHIDDEF_SERVO,				0x6C2, 0x38, { UFINTS(servo,				4) },			0 },
{ PHIDDEF_MOTORCONTROL,			0x6C2, 0x59, { UFINTS(motorcontrol,			2, 0) },		0 }, //HC
{ PHIDDEF_INTERFACEKIT,			0x6C2, 0x81, { UFINTS(ifkit,				0, 0, 8) },		0 },
{ PHIDDEF_ADVANCEDSERVO,		0x6C2, 0x82, { UFINTS(advancedservo,		1) },			0 },
{ 0 }
};
