#ifndef __CPHIDGETLED
#define __CPHIDGETLED
#include "cphidget.h"

/** \defgroup phidled Phidget LED 
 * \ingroup phidgets
 * Calls specific to the Phidget LED. See the product manual for more specific API details, supported functionality, units, etc.
 * @{
 */

DPHANDLE(LED)
CHDRSTANDARD(LED)

/**
 * Gets the number of LEDs supported by this board.
 * @param phid An attached phidget LED handle.
 * @param count The led count.
 */
CHDRGET(LED,LEDCount,int *count)
/**
 * Gets the brightness of an LED.
 * @param phid An attached phidget LED handle.
 * @param index The LED index.
 * @param brightness The LED brightness (0-100).
 */
CHDRGETINDEX(LED,DiscreteLED,int *brightness)
/**
 * Sets the brightness of an LED.
 * @param phid An attached phidget LED handle.
 * @param index The LED index.
 * @param brightness The LED brightness (0-100).
 */
CHDRSETINDEX(LED,DiscreteLED,int brightness)

#ifndef REMOVE_DEPRECATED
DEP_CHDRGET("Deprecated - use CPhidgetLED_getLEDCount",LED,NumLEDs,int *)
#endif

#ifndef EXTERNALPROTO
#define LED_MAXLEDS 64
struct _CPhidgetLED 
{
	CPhidget phid;

	int LED_Power[LED_MAXLEDS];
	int nextLED_Power[LED_MAXLEDS];
	unsigned char changedLED_Power[LED_MAXLEDS];
	unsigned char changeRequests;
} typedef CPhidgetLEDInfo;
#endif

/** @} */

#endif
