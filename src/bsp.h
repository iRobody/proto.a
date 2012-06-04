#ifndef _BSP_H
#define _BSP_H

#include <avr/io.h>  // AVR I/O

#define DEBUG 1

// Sys timer tick per seconds
#define BSP_TICKS_PER_SEC    100
#define MS2TICKS( t)	(BSP_TICKS_PER_SEC / 1000.0 * t  )

#define SAVE_POWER

#define TICK_DIVIDER       ((F_CPU / BSP_TICKS_PER_SEC / 1024) - 1)

#if TICK_DIVIDER > 255
#   error BSP_TICKS_PER_SEC too small
#elif TICK_DIVIDER < 2
#   error BSP_TICKS_PER_SEC too large
#endif

#ifdef DEBUG
#define LOG(s)	Serial.print(TAG);Serial.print(":");Serial.println(s);
#else
#define LOG(s)
#endif

#define USER_LED_ON()      (PORTB |= (1 << (7)))
#define USER_LED_OFF()     (PORTB &= ~(1 << (7)))
#define USER_LED_TOGGLE()  (PORTB ^= (1 << (7)))

//move bsp support
#define MA_DIR 4
#define MA_PWM 5
//motor right at B channel
#define MB_DIR 7
#define MB_PWM 6

void bspMove( bool leftForward, byte leftSpeed, bool rightForward, byte rightSpeed);

#endif //_BSP_H
