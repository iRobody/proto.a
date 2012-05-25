#include <Arduino.h>

#include "moveActive.h"

MoveActive moveActive = MoveActive();
//state handlers ----------------------------
QSTATE_HANDLER_DEF(MoveActive, kick, event) {
  Serial.println("moveActive kickoff");
  return Q_TRAN(QSTATEHANDLER(ready));
}

QSTATE_HANDLER_DEF(MoveActive, ready, event) {
  Serial.println("moveActive ready");
  return Q_SUPER(&QHsm::top);
}


MoveActive::MoveActive()
  :QActive(QSTATEHANDLER(kick))
  ,me(this){
}

QEvent const * MoveActive::eQueue[2];
void MoveActive::start(uint8_t prio) {
  QActive::start( prio, eQueue, Q_DIM(eQueue),(void *)0, 0,0);
}  
//bsp*****
void MoveActive::bspInit() {
}  



