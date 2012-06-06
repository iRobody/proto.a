#include <Arduino.h>
#include <robody/events.h>

#include "bsp.h"
#include "moveActive.h"
#include "moveEvent.h"

static const char* TAG = "MOVE";

MoveActive moveActive = MoveActive();

MoveActive::MoveActive()
  :QActive((QStateHandler)&MoveActive::initial) {
	lF = true;
	lS = 0;
	rF = true;
	rS = 0;
}

static QEvent const * eQueue[2];
void MoveActive::start(uint8_t prio) {
  QActive::start( prio, eQueue, Q_DIM(eQueue),(void *)0, 0,0);
}  
//bsp*****
void MoveActive::bspInit() {
	bspMove( lF, lS, rF, rS);
}  

void MoveActive::direct(bool forward, byte speed) {
	move( forward, speed, forward, speed);
}

void MoveActive::steer( bool left, byte speed) {
	if( left) {
		move( lF, lS-speed > 0 ? lS-speed:0, rF, rS+speed < 255? rS+speed: 255);
	} else {
		move( lF, lS+speed < 255? lS+speed:255, rF, rS-speed > 0? rS-speed: 0);
	}
}

void MoveActive::accelerate( bool accel, byte speed) {
	if( accel) {
		move( lF, lS+speed < 255? lS+speed : 255, rF, rS+speed < 255 ? rS+speed : 255);
	} else {
		move( lF, lS - speed > 0? lS-speed : 0, rF, rS-speed > 0 ? rS-speed : 0);
	}
}

void MoveActive::brake( bool hard) {
	if( hard) {
		move( !lF, 255, !rF, 255);
		delay(10);
		move( !lF,0,!rF,0);
	} else {
		move (lF, 0, rF, 0);
	}
}
void MoveActive::move(bool leftForward, byte leftSpeed, bool rightForward, byte rightSpeed) {
	lF = leftForward;
	lS = leftSpeed;
	rF = rightForward;
	rS = rightSpeed;
	bspMove( lF, lS, rF, rS);
}
//begin: state handlers ............................
QSTATE_HANDLER_DEF(MoveActive, initial, event) {
	subscribe(EVENT_CH_MOVE_C);
	return Q_TRAN(&MoveActive::run);
}

QSTATE_HANDLER_DEF(MoveActive, run, event) {
	switch( event->sig) {
	case MV_SIG_DIRECT:
	{
		MoveDirectEvent* pE = (MoveDirectEvent*)event;
		direct( pE->forward, pE->speed);
	}
		return Q_HANDLED();
	case MV_SIG_STEER:
	{
		MoveSteerEvent* pE = (MoveSteerEvent*)event;
		steer( pE->left, pE->speed);
	}
		return Q_HANDLED();
	case MV_SIG_ACCEL:
	{
		MoveAccelEvent* pE = (MoveAccelEvent*)event;
		accelerate( pE->accel, pE->speed);
	}
		return Q_HANDLED();
	case MV_SIG_BRAKE:
	{
		MoveBrakeEvent* pE = (MoveBrakeEvent*)event;
		brake( pE->hard);
	}
		return Q_HANDLED();
	case MV_SIG_RAW:
	{
		MoveRawEvent* pE = (MoveRawEvent*)event;
		move( pE->lF, pE->lS, pE->rF, pE->rS);
	}
		return Q_HANDLED();
	case Q_INIT_SIG:
		return Q_HANDLED();
	}
	LOG("run");
  return Q_SUPER(&QHsm::top);
}






//end: state handler ..................................
