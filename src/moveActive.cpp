#include <Arduino.h>
#include <irobody/events.h>

#include "bsp.h"
#include "moveActive.h"

static const char* TAG = "MOVE";

MoveActive moveActive = MoveActive();

MoveActive::MoveActive()
  :QActive(QSTATEHANDLER(initial)) {
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
	subscribe( SIG_MOVE_C);
	return Q_TRAN(QSTATEHANDLER(run));
}

QSTATE_HANDLER_DEF(MoveActive, run, event) {
	switch( event->sig) {
	case SIG_MOVE_C:
		LOG( "get one command");
		return Q_HANDLED();
	case Q_ENTRY_SIG:
	case Q_INIT_SIG:
		return Q_HANDLED();
	}
  return Q_SUPER(&QHsm::top);
}






//end: state handler ..................................
