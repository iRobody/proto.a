#include <Arduino.h>
#include "bsp.h"
#include "accessoryActive.h"

static const char* TAG = "accessory";

AccessoryActive accessoryActive = AccessoryActive();

AndroidAccessory AccessoryActive::acc = AndroidAccessory("iRobuddy, Inc.",
	     "iRobody",
	     "model proto.alpha",
	     "1.0",
	     "http://www.irobuddy.com",
	     "0000000012345678");

void AccessoryActive::intHandler() {
	acc.intHandler();
}

AccessoryActive::AccessoryActive()
  :QActive( QSTATEHANDLER(initial)) {
}

static QEvent const *eQueue[2];
void AccessoryActive::start( byte prio) {
  QActive::start( prio, eQueue, Q_DIM(eQueue),(void *)0, 0);
}

void AccessoryActive::bspInit() {
	acc.powerOn();
	attachInterrupt(INT6, AccessoryActive::intHandler, LOW);
}

//begin: state handlers ...........................................
QSTATE_HANDLER_DEF(AccessoryActive, initial, event) {
  return Q_TRAN(QSTATEHANDLER(run));
}

QSTATE_HANDLER_DEF(AccessoryActive, run, event) {
  return Q_SUPER(&QHsm::top);
}

//end: state handlers .............................................






