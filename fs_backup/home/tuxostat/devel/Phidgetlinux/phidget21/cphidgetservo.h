#ifndef __CPHIDGETSERVO
#define __CPHIDGETSERVO
#include "cphidget.h"

/** \defgroup phidservo Phidget Servo 
 * \ingroup phidgets
 * Calls specific to the Phidget Servo. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */

DPHANDLE(Servo)
CHDRSTANDARD(Servo)

/**
 * Gets the number of motors supported by this controller.
 * @param phid An attached phidget servo handle.
 * @param count The motor count.
 */
CHDRGET(Servo,MotorCount,int *count)

/**
 * Gets the current position of a motor.
 * @param phid An attached phidget servo handle.
 * @param index The motor index.
 * @param position The motor position.
 */
CHDRGETINDEX(Servo,Position,double *position)
/**
 * Sets the current position of a motor.
 * @param phid An attached phidget servo handle.
 * @param index The motor index.
 * @param position The motor position.
 */
CHDRSETINDEX(Servo,Position,double position)
/**
 * Gets the maximum position that a motor can be set to.
 * @param phid An attached phidget servo handle.
 * @param index The motor index.
 * @param max The maximum position.
 */
CHDRGETINDEX(Servo,PositionMax,double *max)
/**
 * Gets the minimum position that a motor can be set to.
 * @param phid An attached phidget servo handle.
 * @param index The motor index.
 * @param min The minimum position.
 */
CHDRGETINDEX(Servo,PositionMin,double *min)
/**
 * Sets a position change event handler. This is called when the position changes.
 * @param phid An attached phidget servo handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
CHDREVENTINDEX(Servo,PositionChange,double position)
/**
 * Gets the engaged state of a motor. This is whether the motor is powered or not.
 * @param phid An attached phidget servo handle
 * @param index The motor index.
 * @param engagedState The engaged state. Possible values are \ref PTRUE and \ref PFALSE.
 */
CHDRGETINDEX(Servo,Engaged,int *engagedState)
/**
 * Sets the engaged state of a motor. This is whether the motor is powered or not.
 * @param phid An attached phidget servo handle
 * @param index The motor index.
 * @param engagedState The engaged state. Possible values are \ref PTRUE and \ref PFALSE.
 */
CHDRSETINDEX(Servo,Engaged,int engagedState)

#ifndef REMOVE_DEPRECATED
DEP_CHDRGET("Deprecated - use CPhidgetServo_getMotorCount",Servo,NumMotors,int *)
DEP_CHDRSETINDEX("Deprecated - use CPhidgetServo_setPosition",Servo,MotorPosition,double)
DEP_CHDRGETINDEX("Deprecated - use CPhidgetServo_getPosition",Servo,MotorPosition,double *)
DEP_CHDRGETINDEX("Deprecated - use CPhidgetServo_getPositionMax",Servo,MotorPositionMax,double *)
DEP_CHDRGETINDEX("Deprecated - use CPhidgetServo_getPositionMin",Servo,MotorPositionMin,double *)
DEP_CHDREVENTINDEX("Deprecated - use CPhidgetServo_set_OnPositionChange_Handler",Servo,MotorPositionChange,double motorPosition)
DEP_CHDRSETINDEX("Deprecated - use CPhidgetServo_setEngaged",Servo,MotorOn,int)
DEP_CHDRGETINDEX("Deprecated - use CPhidgetServo_getEngaged",Servo,MotorOn,int *)
#endif

#ifndef EXTERNALPROTO
#define SERVO_MAXSERVOS 4
struct _CPhidgetServo {
	CPhidget phid;

	int (CCONV *fptrPositionChange)(CPhidgetServoHandle, void *, int, double);           
	void *fptrPositionChangeptr;

	//Deprecated
	int (CCONV *fptrMotorPositionChange)(CPhidgetServoHandle, void *, int, double);           
	void *fptrMotorPositionChangeptr;

	double motorPositionEcho[SERVO_MAXSERVOS];
	unsigned char motorEngagedStateEcho[SERVO_MAXSERVOS];

	double motorPosition[SERVO_MAXSERVOS];
	unsigned char motorEngagedState[SERVO_MAXSERVOS];

	unsigned char fullStateEcho;

	double motorPositionMax, motorPositionMin;

	unsigned char outputPacket[8];
	unsigned int outputPacketLen;
} typedef CPhidgetServoInfo;
#endif

/** @} */

#endif
