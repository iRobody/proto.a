#include <arduino.h>

#include "bsp.h"//motor left at A channel

void bspWheelMove( bool leftForward, byte leftSpeed, bool rightForward, byte rightSpeed) {
	digitalWrite( MA_DIR, leftForward? LOW:HIGH);
	digitalWrite( MB_DIR, rightForward? LOW:HIGH);
	analogWrite( MA_PWM, leftSpeed);
	analogWrite( MB_PWM, rightSpeed);
}
