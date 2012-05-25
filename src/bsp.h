#ifndef _BSP_H
#define _BSP_H

#include <avr/io.h>  // AVR I/O

// Sys timer tick per seconds
#define BSP_TICKS_PER_SEC    100

//#define SAVE_POWER

#define USER_LED_ON()      (PORTB |= (1 << (5)))
#define USER_LED_OFF()     (PORTB &= ~(1 << (5)))
#define USER_LED_TOGGLE()  (PORTB ^= (1 << (5)))

#define TICK_DIVIDER       ((F_CPU / BSP_TICKS_PER_SEC / 1024) - 1)

#if TICK_DIVIDER > 255
#   error BSP_TICKS_PER_SEC too small
#elif TICK_DIVIDER < 2
#   error BSP_TICKS_PER_SEC too large
#endif


#endif //_BSP_H
