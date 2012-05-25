#include <Arduino.h>

#include "accessoryActive.h"

#define INT_VECTOR_NUM 15

AccessoryActive accessoryActive = AccessoryActive();

//need by QF
//extern QActive* superActive = &aActive;

AndroidAccessory AccessoryActive::acc = AndroidAccessory("iRobuddy, Inc.",
		     "irobody",
		     "irobuddy basic dock body",
		     "1.0",
		     "http://www.irobuddy.com",
		     "0000000012345678");


//state handlers ...........................................
QSTATE_HANDLER_DEF(AccessoryActive, kick, event) {
  Serial.println("AccessoryActive kickoff");
  return Q_TRAN(QSTATEHANDLER(ready));
}

QSTATE_HANDLER_DEF(AccessoryActive, ready, event) {
  Serial.println("AccessoryActive ready");
  return Q_SUPER(&QHsm::top);
}


AccessoryActive::AccessoryActive()
  :QActive( QSTATEHANDLER(kick))
  ,me(this){
}

QEvent const *AccessoryActive::eQueue[2];
void AccessoryActive::start( byte prio) {
  QActive::start( prio, eQueue, Q_DIM(eQueue),(void *)0, 0);
}

//int handler ..............................................
void AccessoryActive::intHandler() {
  //check usb buffer
  Serial.println("superActive in ISR now");
  acc.intHandler();
  //publish task
}

void AccessoryActive::bspInit() {
  attachInterrupt( INT_VECTOR_NUM, intHandler, LOW);
  acc.powerOn();
}




