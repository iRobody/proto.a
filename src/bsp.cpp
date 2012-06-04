#include <arduino.h>
#include "bsp.h"//motor left at A channel

void bspMove( bool leftForward, byte leftSpeed, bool rightForward, byte rightSpeed) {
	digitalWrite( MA_DIR, leftForward? HIGH:LOW);
	digitalWrite( MB_DIR, rightForward? HIGH:LOW);
	analogWrite( MA_PWM, leftSpeed);
	analogWrite( MB_PWM, rightSpeed);
}
