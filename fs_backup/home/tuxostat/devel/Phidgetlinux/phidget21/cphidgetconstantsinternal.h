#ifndef CPHIDGET_CONSTANTS_INTERNAL
#define CPHIDGET_CONSTANTS_INTERNAL

//internal status representation via flags - for Phidget, Manager
/*
 * PHIDGET_ATTACHED_FLAG	
 *	true:	Phidget is attached to computer, and you can call all commands on this handle
 *	false:	Phidget is not ready to handle calls yet - might be not connected, 
 *			in proccess of connecting, just disconnected, not initialized, etc.
 *	This flag is returend externally to users.
 */
#define PHIDGET_ATTACHED_FLAG			0x01
/*
 * PHIDGET_DETACHING_FLAG	
 *	true:	Phidget has just been detached, but the detach code is still running
 *	false:	Any other time
 *	There are two types of funtions - those that communicate with devices and those that
 *	return cached info - the second class should succedd if this is true and PHIDGET_ATTACHED_FLAG is false,
 *	so we can get device info in the detach handler.
 */
#define PHIDGET_DETACHING_FLAG			0x02
/*
 * PHIDGET_USB_ERROR_FLAG	
 *	true:	Phidget has encountered a USB error bad enough to require a detach
 *	false:	Any other time
 *	The base thread will notice this error code and detach the device. If it is ok, it will
 *  be picked up next iteration and an attach will occur.
 */
#define PHIDGET_USB_ERROR_FLAG			0x04
/*
 * PHIDGET_ATTACHING_FLAG	
 *	true:	Phidget is attached to computer, but initialization is still in progress
 *	false:	any other time
 *	This is used internally only
 */
#define PHIDGET_ATTACHING_FLAG			0x08
/*
 * PHIDGET_OPENED_FLAG	
 *	true:	open has been called on the handle
 *	false:	open has not yet been called, or close has been called.
 *	This is used internally only
 */
#define PHIDGET_OPENED_FLAG			0x10

//internal status representation via flags - for socket
/*
 * PHIDGETSOCKET_ATTACHED_FLAG	
 *	true:	Socket is open
 *	false:	Socket is not open
 *	This flag is returend externally to users.
 */
#define PHIDGETSOCKET_ATTACHED_FLAG		0x01
/* states for the socket status variable */
#define PHIDGETSOCKET_CONNECTIONERROR_FLAG	0x02

#define MAX_IN_PACKET_SIZE				64
#define MAX_OUT_PACKET_SIZE				32

#define MAXDEVICETYPELENGTH				50

/* Internal device-class type */
#define PHIDGET_DEVICE_TYPE_COUNT		23
#define PHIDDEF_NOTHING_00				0x00
#define PHIDDEF_NOTHING_01				0x01
#define PHIDDEF_NOTHING_02				0x02
#define PHIDDEF_INTERFACEKIT			0x03
#define PHIDDEF_SERVO					0x04
#define PHIDDEF_HUMIDITYSENSOR			0x05
#define PHIDDEF_NOTHING_06				0x06
#define PHIDDEF_RFID					0x07
#define PHIDDEF_WEIGHTSENSOR			0x08
#define PHIDDEF_NOTHING_09				0x09
#define PHIDDEF_TEXTLCD					0x0A
#define PHIDDEF_MOTORCONTROL			0x0B
#define PHIDDEF_NOTHING_0C				0x0C
#define PHIDDEF_LED						0x0D
#define PHIDDEF_ENCODER					0x0E
#define PHIDDEF_STEPPER					0x0F
#define PHIDDEF_TEMPERATURESENSOR		0x10
#define PHIDDEF_ACCELEROMETER			0x11
#define PHIDDEF_TEXTLED					0x12
#define PHIDDEF_PHSENSOR				0x13
#define PHIDDEF_GYROSCOPE				0x14
#define PHIDDEF_ADVANCEDSERVO			0x15
#define PHIDDEF_GPS						0x16

/* Internal device-specific type */
#define PHIDGET_DEVICE_COUNT			45
#define PHIDSPEC_NOTHING				0x00
#define PHIDSPEC_SERVO_1MOTOR			0x01
#define PHIDSPEC_INTERFACEKIT_8_8_8		0x02
#define PHIDSPEC_INTERFACEKIT_0_0_4		0x03
#define PHIDSPEC_INTERFACEKIT_0_16_16	0x04
#define PHIDSPEC_INTERFACEKIT_0_5_7		0x05
#define PHIDSPEC_INTERFACEKIT_0_8_8		0x06
#define PHIDSPEC_ACCELEROMETER			0x07
#define PHIDSPEC_TEXTLCD_2_20			0x08
#define PHIDSPEC_TEXTLCD_2_20_CUSTOM	0x09
#define PHIDSPEC_TEXTLCD_2_20_COMP1		0x0A
#define PHIDSPEC_RFID					0x0B
#define PHIDSPEC_LED					0x0C
#define PHIDSPEC_TEXTLED				0x0D
#define PHIDSPEC_MOTORCONTROL			0x0E
#define PHIDSPEC_TEMPERATURESENSOR		0x0F
#define PHIDSPEC_STEPPER				0x10
#define PHIDSPEC_ENCODER				0x11
#define PHIDSPEC_INTERFACEKIT_4_8_8		0x12
#define PHIDSPEC_SERVO_1MOTOR_OLD		0x13
#define PHIDSPEC_SERVO_4MOTOR_OLD		0x14
#define PHIDSPEC_WEIGHTSENSOR			0x15
#define PHIDSPEC_HUMIDITYSENSOR			0x16
#define PHIDSPEC_PHSENSOR				0x17
#define PHIDSPEC_GYROSCOPE				0x18
#define PHIDSPEC_ADVANCEDSERVO8			0x19
#define PHIDSPEC_RFIDB					0x1A
#define PHIDSPEC_TEXTLED2				0x1B
#define PHIDSPEC_LINEAR_TOUCH			0x1C
#define PHIDSPEC_ROTARY_TOUCH			0x1D
#define PHIDSPEC_ENCODER_4				0x1E
#define PHIDSPEC_GPS					0x1F
#define PHIDSPEC_ENCODER_HS				0x20
#define PHIDSPEC_UNIPOLAR_STEPPER		0x21
#define PHIDSPEC_BIPOLAR_STEPPER		0x22
#define PHIDSPEC_JOYSTICK				0x23
#define PHIDSPEC_TEXTLCD_with_888		0x24
#define PHIDSPEC_888_with_lcd			0x25
#define PHIDSPEC_ACCELEROMETER3			0x26
#define PHIDSPEC_ACCELEROMETER_w_GYRO	0x27
#define PHIDSPEC_GYRO_w_ACCELEROMETER	0x28
#define PHIDSPEC_SERVO_4MOTOR			0x29
#define PHIDSPEC_MOTORCONTROL_HC		0x2A
#define PHIDSPEC_INTERFACEKIT_0_0_8		0x2B
#define PHIDSPEC_ADVANCEDSERVO1			0x2C

#endif

