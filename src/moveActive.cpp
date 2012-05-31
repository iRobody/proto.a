#include <Arduino.h>
#include <robody/events.h>

#include "bsp.h"
#include "moveActive.h"

static const char* TAG = "MOVE";

MoveActive moveActive = MoveActive();

MoveActive::MoveActive()
  :QActive((QStateHandler)&MoveActive::initial) {
}

static QEvent const * eQueue[2];
void MoveActive::start(uint8_t prio) {
  QActive::start( prio, eQueue, Q_DIM(eQueue),(void *)0, 0,0);
}  
//bsp*****
void MoveActive::bspInit() {
}  

//begin: state handlers ............................
QSTATE_HANDLER_DEF(MoveActive, initial, event) {
	subscribe(EVENT_CH_MOVE_C);
	return Q_TRAN(&MoveActive::run);
}

QSTATE_HANDLER_DEF(MoveActive, run, event) {
	switch( event->sig) {
	case MV_SIG_FORWARD:
		LOG( "get one command");
		return Q_HANDLED();
	case Q_ENTRY_SIG:
	case Q_INIT_SIG:
		LOG("init run");
		return Q_HANDLED();
	}
	LOG("run");
  return Q_SUPER(&QHsm::top);
}






//end: state handler ..................................
