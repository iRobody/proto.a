#include <Arduino.h>
#include <qp/events.h>

#include "bsp.h"
#include "moveActive.h"
#include "moveEvent.h"

#if DEBUG
static const char* TAG = "accessory";
#endif

#define SPEED_BASE 10
#define SPEED_MAX 127
#define SPEED_STEER_DIF 10

MoveActive moveActive = MoveActive();

MoveActive::MoveActive()
  :QActive((QStateHandler)&MoveActive::initial) {
	leftSpeed = 0;
	rightSpeed = 0;
}

static QEvent const * eQueue[2];
void MoveActive::start(uint8_t prio) {
  QActive::start( prio, eQueue, Q_DIM(eQueue),(void *)0, 0,0);
}

//bsp*****
void MoveActive::bspInit()
{
	bspWheelMove( 0,0,0,0);
}  

void MoveActive::steer( signed char direction)
{
	int leftSpeed_gap = 0;
	signed short rightSpeed_gap = 0;
	if( direction < 0) {
		leftSpeed_gap = leftSpeed + SPEED_MAX;
		rightSpeed_gap = SPEED_MAX - rightSpeed;
		rightSpeed_gap = (leftSpeed_gap>rightSpeed_gap?rightSpeed_gap:leftSpeed_gap);
		leftSpeed_gap = SPEED_STEER_DIF * -1 * direction;
		rightSpeed_gap = (leftSpeed_gap>rightSpeed_gap?rightSpeed_gap:leftSpeed_gap);
		move(leftSpeed-rightSpeed_gap, rightSpeed+rightSpeed_gap);
	} else if ( direction > 0){
		leftSpeed_gap = SPEED_MAX - leftSpeed;
		rightSpeed_gap = rightSpeed + SPEED_MAX;
		rightSpeed_gap = (leftSpeed_gap>rightSpeed_gap?rightSpeed_gap:leftSpeed_gap);
		leftSpeed_gap = SPEED_STEER_DIF*direction;
		rightSpeed_gap = (leftSpeed_gap>rightSpeed_gap?rightSpeed_gap:leftSpeed_gap);
		move(leftSpeed+rightSpeed_gap, rightSpeed-rightSpeed_gap);
	} else {
		leftSpeed_gap = (leftSpeed+rightSpeed)/2;
		move( leftSpeed_gap, leftSpeed_gap);
	}
}

void MoveActive::accelerate( signed char speed)
{
	int speed_gap = 0;
	if( speed > 0) {
		speed_gap = SPEED_MAX - (leftSpeed>rightSpeed?leftSpeed:rightSpeed);
		speed_gap = (speed_gap>speed?speed:speed_gap);
	} else {
		speed_gap = -SPEED_MAX - (leftSpeed>rightSpeed?rightSpeed:leftSpeed);
		speed_gap = (speed_gap>speed?speed_gap:speed);
	}
	move( leftSpeed+speed_gap, rightSpeed+speed_gap);
}

void MoveActive::brake( unsigned char level) {
	if( level == 1) {
		move( leftSpeed>0?-127:127, rightSpeed>0?-127:127);
		delay(10);
		move( 0,0);
	} else {
		move(0, 0);
	}
}
void MoveActive::move( signed char leftSpeed, signed char rightSpeed) {
	bool lF = (leftSpeed>0);
	byte lS = (lF?1:-1)*(leftSpeed + (leftSpeed==0?0:(lF?1:-1))*SPEED_BASE);
	bool rF = (rightSpeed>0);
	byte rS = (rF?1:-1)*(rightSpeed + (rightSpeed==0?0:(rF?1:-1))*SPEED_BASE);
	this->leftSpeed = leftSpeed;
	this->rightSpeed = rightSpeed;
	bspWheelMove( lF, lS, rF, rS);
#if DEBUG
	Serial.print("move:(");
	Serial.print((int)leftSpeed);
	Serial.print(",");
	Serial.print((int)rightSpeed);
	Serial.print(")-(");
	Serial.print((int)lS);
	Serial.print(",");
	Serial.print((int)rS);
	Serial.println(")");
#endif

}

//begin: state handlers ............................
QSTATE_HANDLER_DEF(MoveActive, initial, event) {
	subscribe(EVENT_CH_MOVE_C);
	return Q_TRAN(&MoveActive::run);
}

QSTATE_HANDLER_DEF(MoveActive, run, event) {
	switch( event->sig) {
	case MV_SIG_RAW:
	{
		MoveRawEvent* pE = (MoveRawEvent*)event;
		move( pE->leftSpeed, pE->rightSpeed);
	}
		return Q_HANDLED();
	case MV_SIG_STEER:
	{
		MoveSteerEvent* pE = (MoveSteerEvent*)event;
		steer( pE->direction);
	}
		return Q_HANDLED();
	case MV_SIG_ACCEL:
	{
		MoveAccelEvent* pE = (MoveAccelEvent*)event;
		accelerate( pE->speed);
	}
		return Q_HANDLED();
	case MV_SIG_BRAKE:
	{
		MoveBrakeEvent* pE = (MoveBrakeEvent*)event;
		brake( pE->level);
	}
		return Q_HANDLED();
	case Q_INIT_SIG:
		return Q_HANDLED();
	}
  return Q_SUPER(&QHsm::top);
}

//end: state handler ..................................
