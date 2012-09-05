#ifndef _BSP_H
#define _BSP_H

#include <avr/io.h>  // AVR I/O
#include "configure.h"

// System timer tick per seconds
#define BSP_TICKS_PER_SEC    100
#define MS2TICKS( t)	(BSP_TICKS_PER_SEC / 1000.0 * t  )

#define SAVE_POWER

#define TICK_DIVIDER       ((F_CPU / BSP_TICKS_PER_SEC / 1024) - 1)

#if TICK_DIVIDER > 255
#   error BSP_TICKS_PER_SEC too small
#elif TICK_DIVIDER < 2
#   error BSP_TICKS_PER_SEC too large
#endif

#if DEBUG
#define LOG(s)	QF_INT_DISABLE();Serial.print(TAG);Serial.print(":");Serial.println(s);QF_INT_ENABLE()
#else
#define LOG(s)
#endif

#define USER_LED_ON()      (PORTB |= (1 << (7)))
#define USER_LED_OFF()     (PORTB &= ~(1 << (7)))
#define USER_LED_TOGGLE()  (PORTB ^= (1 << (7)))

//--------------MOVE BSP
#define MA_DIR 4
#define MA_PWM 5
//motor right at B channel
#define MB_DIR 7
#define MB_PWM 6
void bspWheelMove( bool leftForward, byte leftSpeed, bool rightForward, byte rightSpeed);

//--------------RANGE BSP
#define RANGE_INT_VECTOR	PCINT0_vect
#define RANGE_INT_PCMSK	0xF0
#define RANGE_INT_PCICR		0x01
#define RANGE_FRONT_LEFT_PIN	13
#define RANGE_FRONT_RIGHT_PIN	12
#define RANGE_REAR_LEFT_PIN		11
#define RANGE_REAR_RIGHT_PIN	10

#endif //_BSP_H
