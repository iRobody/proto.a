#include <Arduino.h>
#include <qp/events.h>

#include "bsp.h"
#include "moveEvent.h"
#include "rangeEvent.h"
#include "rangeActive.h"

#if DEBUG
static const char* TAG = "RANGE";
#endif

RangeActive rangeActive = RangeActive();

#define GUARD_EVENT_FREQ_SPAN 200

static unsigned long lastUpdateTime = 0;
static RangeEdgeEvent edgeEvent = {QEVENT_RELAY_PUB_(EVENT_CH_RANGE_S, RNG_SIG_EDGE, RANGE_EDGE_DATA_LEN)};
static MoveBrakeEvent brakeEvent = {QEVENT_PUB_(EVENT_CH_MOVE_C,MV_SIG_BRAKE, MOVE_BRAKE_DATA_LEN),1};

// ISR --------------------------------------------
ISR(RANGE_INT_VECTOR) {
	unsigned long time = millis();
	if( (time - lastUpdateTime) < GUARD_EVENT_FREQ_SPAN)
		return;
	lastUpdateTime = time;
	edgeEvent.frontRight = digitalRead(RANGE_FRONT_RIGHT_PIN);
	edgeEvent.frontLeft = digitalRead(RANGE_FRONT_LEFT_PIN);
	edgeEvent.rearRight = digitalRead(RANGE_REAR_RIGHT_PIN);
	edgeEvent.rearLeft = digitalRead(RANGE_REAR_LEFT_PIN);
	if( *((unsigned long*)&edgeEvent.frontLeft)!=0)
		QF::publish(&brakeEvent.e);
	//TODO: it not work, unless QP count refCtr even for static objects
	if( edgeEvent.e.refCtr_ == 0)
		QF::publish(&edgeEvent.e);
	//if auto-cruising
	/*
	if( rangeActive.cruising)
		rangeActive.postFIFO(&brakeEvent.e);
	*/
}

RangeActive::RangeActive()
  :QActive((QStateHandler)&RangeActive::initial) {

}

static QEvent const * eQueue[2];
void RangeActive::start(uint8_t prio) {
  QActive::start( prio, eQueue, Q_DIM(eQueue),(void *)0, 0,0);
}
// BSP initial ------------------------------
void RangeActive::bspInit() {
	pinMode( RANGE_FRONT_LEFT_PIN, INPUT);
	pinMode( RANGE_FRONT_RIGHT_PIN, INPUT);
	pinMode( RANGE_REAR_LEFT_PIN, INPUT);
	pinMode( RANGE_REAR_RIGHT_PIN, INPUT);

	PCMSK0 = RANGE_INT_PCMSK;

	PCICR = RANGE_INT_PCICR; //enable pcint0 interrupt vector
}

//BEGIN: state handlers -----------------------------
QSTATE_HANDLER_DEF(RangeActive, initial, event) {
	subscribe(EVENT_CH_MOVE_C);
	return Q_TRAN(&RangeActive::run);
}

QSTATE_HANDLER_DEF(RangeActive, run, event) {
	switch(  event->sig) {
	case Q_INIT_SIG:
	default:
		break;
	}
	return Q_SUPER(&QHsm::top);
}
//END: state handlers -----------------------------
