#include <Arduino.h>

#include <qp/qp_port.h>
#include <robody/events.h>

#include "bsp.h"
#include "accessoryActive.h"
#include "moveActive.h"

// ISRs ----------------------------------------------------------------------
ISR(TIMER2_COMPA_vect) {

    // No need to clear the interrupt source since the Timer2 compare
    // interrupt is automatically cleard in hardware when the ISR runs.
    QF::TICK(&l_TIMER2_COMPA);                // process all armed time events
}

//............................................................................
void QF::onStartup(void) {

	// set Timer2 in CTC mode, 1/1024 prescaler, start the timer ticking
    TCCR2A = (1 << WGM21) | (0 << WGM20);
    TCCR2B = (1 << CS22 ) | (1 << CS21) | (1 << CS20);               // 1/2^10
    ASSR &= ~(1 << AS2);
    TIMSK2 = (1 << OCIE2A);                 // Enable TIMER2 compare Interrupt
    TCNT2 = 0;

    OCR2A = TICK_DIVIDER;     // must be loaded last for Atmega168 and friends
}
//............................................................................
void QF::onCleanup(void) {
}
//............................................................................
void QF::onIdle() {

#ifdef SAVE_POWER

    SMCR = (0 << SM0) | (1 << SE);  // idle sleep mode, adjust to your project

    // never separate the following two assembly instructions, see NOTE2
    __asm__ __volatile__ ("sei" "\n\t" :: );
    __asm__ __volatile__ ("sleep" "\n\t" :: );

    SMCR = 0;                                              // clear the SE bit
   // Serial.println("onIdle");
#else
    QF_INT_ENABLE();
#endif
}
//............................................................................
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    QF_INT_DISABLE();                                // disable all interrupts
    while(1) {
    	USER_LED_TOGGLE();
    	delay(10000);
    }
    asm volatile ("jmp 0x0000");    // perform a software reset of the Arduino
}

//............................................................................
void BSP_init(void) {
	DDRB  = 0xFF;                     // All PORTB pins are outputs (user LED)
	PORTB = 0x00;

  Serial.begin(115200);   // set the highest stanard baud rate of 115200 bps

  moveActive.bspInit();
  accessoryActive.bspInit();
}

// Local-scope objects -------------------------------------------------------
static QSubscrList   subscrSto[MAX_PUB_EVENT_CH];

static union MinEvents {
    void   *e0;                                          // minimum event size
    uint8_t e1[sizeof(QEvent)];
    // ... other event types to go into this pool
} minEPool[8];                        // storage for the small event pool

static union MaxEvents {
    void   *e0;                                          // minimum event size
    uint8_t e1[sizeof(QEventMax)];
    // ... other event types to go into this pool
} maxEPool[16];

void setup(){
  BSP_init(); 
  QF::init();       // initialize the framework and the underlying RT kernel
  // initialize event pools...
  QF::poolInit(minEPool, sizeof(minEPool), sizeof(minEPool[0]));
  QF::poolInit(maxEPool, sizeof(maxEPool), sizeof(maxEPool[0]));
  QF::psInit(subscrSto, Q_DIM(subscrSto));     // init publish-subscribe

  // start the active objects...
  moveActive.start(2);
  accessoryActive.start(1);
}
  
