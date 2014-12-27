#ifndef __CPHIDGETENCODER
#define __CPHIDGETENCODER
#include "cphidget.h"

/** \defgroup phidenc Phidget Encoder 
 * \ingroup phidgets
 * Calls specific to the Phidget Encoder. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */

DPHANDLE(Encoder)
CHDRSTANDARD(Encoder)

/**
 * Gets the number of digital inputs supported by this board.
 * @param phid An attached phidget encoder handle
 * @param count The input count.
 */
CHDRGET(Encoder,InputCount,int *count)
/**
 * Gets the state of a digital input.
 * @param phid An attached phidget encoder handle
 * @param index The input index.
 * @param inputState The input state. Possible values are \ref PTRUE and \ref PFALSE.
 */
CHDRGETINDEX(Encoder,InputState,int *inputState)
/**
 * Sets an input change handler. This is called when a digital input changes.
 * @param phid An attached phidget encoder handle
 * @param fptr Callback function pointer.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
CHDREVENTINDEX(Encoder,InputChange,int inputState)

/**
 * Gets the number of encoder inputs supported by this board.
 * @param phid An attached phidget encoder handle
 * @param count The encoder input count.
 */
CHDRGET(Encoder,EncoderCount,int *count)
/**
 * Gets the current position of an encoder.
 * @param phid An attached phidget encoder handle
 * @param index The encoder input index.
 * @param position The current position
 */
CHDRGETINDEX(Encoder,Position,int *position)
/**
 * Sets the current position of an encoder.
 * @param phid An attached phidget encoder handle
 * @param index The encoder input index.
 * @param position The new position
 */
CHDRSETINDEX(Encoder,Position,int position)
/**
 * Sets an encoder change handler. This is called when an encoder position changes.
 * @param phid An attached phidget encoder handle
 * @param fptr Callback function pointer. Note that positionChange is a relative not absolute change and time is the time
 *	in ms since the last position change event.
 * @param userPtr A pointer for use by the user - this value is passed back into the callback function.
 */
CHDREVENTINDEX(Encoder,PositionChange,int time,int positionChange)

#ifndef REMOVE_DEPRECATED
DEP_CHDRGETINDEX("Deprecated - use CPhidgetEncoder_getPosition",Encoder,EncoderPosition,int *)
DEP_CHDRSETINDEX("Deprecated - use CPhidgetEncoder_setPosition",Encoder,EncoderPosition,int)
DEP_CHDRGET("Deprecated - use CPhidgetEncoder_getInputCount",Encoder,NumInputs,int *)
DEP_CHDRGET("Deprecated - use CPhidgetEncoder_getEncoderCount",Encoder,NumEncoders,int *)
#endif

#ifndef EXTERNALPROTO
#define ENCODER_MAXENCODERS 4
#define ENCODER_MAXINPUTS 4
struct _CPhidgetEncoder {
	CPhidget phid;

	int (CCONV *fptrInputChange)        (CPhidgetEncoderHandle, void *, int, int); 
	void *fptrInputChangeptr;
	int (CCONV *fptrPositionChange)        (CPhidgetEncoderHandle, void *, int, int, int); 
	void *fptrPositionChangeptr;

	unsigned char inputState[ENCODER_MAXINPUTS];

	int encoderPosition[ENCODER_MAXENCODERS];
	int encoderTimeStamp[ENCODER_MAXENCODERS];
} typedef CPhidgetEncoderInfo;
#endif

/** @} */

#endif
