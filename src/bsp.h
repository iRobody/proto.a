#ifndef _BSP_H
#define _BSP_H

#include <avr/io.h>  // AVR I/O

#define DEBUG 1

// Sys timer tick per seconds
#define BSP_TICKS_PER_SEC    100

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


#endif //_BSP_H