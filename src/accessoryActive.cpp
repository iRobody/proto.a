#include <Arduino.h>
#include <robody/events.h>

#include "bsp.h"
#include "accessoryActive.h"

static const char* TAG = "accessory";

AccessoryActive accessoryActive = AccessoryActive();

void AccessoryActive::IntHandler() {
	accessoryActive.intHandler();
}

static QEvent busEvent = QEVENT(ACC_SIG_BUSEVENT);
static QEvent detEvent = QEVENT(ACC_SIG_DETACH);
static QEvent attEvent = QEVENT(ACC_SIG_ATTACH);
static QEvent sofEvent = QEVENT(ACC_SIG_SOF);

static QTimeEvt timeE = QTimeEvt(EVENT_SIG_TIMEOUT);

void AccessoryActive::intHandler() {
	byte irq = usb.IntHandler();

	if( irq & bmBUSEVENTIRQ) {
		usb.disInt(bmBUSEVENTIE);
		postFIFO(&busEvent);
		return;
	}

	if( irq & bmCONDETIRQ) {
		if( usb.devAttached()) {
			postFIFO(&attEvent);
		}
		else{
			postFIFO(&detEvent);
		}
		return;
	}

	if( irq & bmFRAMEIRQ) {
		usb.disSOF();
		postFIFO(&sofEvent);
		return;
	}

	if( irq & bmRCVDAVIRQ) {
		QEvent* pEvent = Q_NEW(QEventMax,0);
		char* buff = (char*)&pEvent->type;
		while( pEvent && readFIFO( buff, EVENT_HEADER_SIZE)) {
			buff = (char*)&(pEvent->type) + EVENT_HEADER_SIZE;
			if( pEvent->length)
				if(!readFIFO( buff, pEvent->length))
					break;
			pEvent->type = 0;
			QF::publish(pEvent);
			//request for next package
			reqIn();
			return;
		}
		if( pEvent)
			QF::gc( pEvent);
	}

	if( irq & bmHXFRDNIRQ) {
		//read data failed,retry later
		timeE.postIn(this, MS2TICKS(20));
		return;
	}
}

AccessoryActive::AccessoryActive()
  :QActive( (QStateHandler)(&AccessoryActive::initial))
	,AndroidAccessory("iRobuddy, Inc.",
		     "iRobody",
		     "iRobody model: proto.alpha",
		     "1.0",
		     "http://www.irobuddy.com",
		     "0000000012345678") {
}

static QEvent const *eQueue[8];
void AccessoryActive::start( byte prio) {
  QActive::start( prio, eQueue, Q_DIM(eQueue),(void *)0, 0);
}

void AccessoryActive::bspInit() {
	powerOn();
	LOG("power on ");
	attachInterrupt(INT6, AccessoryActive::IntHandler, LOW);
}


bool AccessoryActive::relayEvent( const QEvent* event, bool enable) {
	//relay event
	if(!(event->type & EVENT_TYPE_RELAY))
			return false;
	if( enable) {
		LOG("relay event");
		write( (char*)&(event->type), event->length + EVENT_HEADER_SIZE);
	}
	return true;
}
//begin: state handlers ............................................
QSTATE_HANDLER_DEF(AccessoryActive, initial, event) {
	subscribe(EVENT_CH_ACC_C);
	if(usb.devAttached()) {
		return Q_TRAN( &AccessoryActive::usb_settle);
	}
	return Q_TRAN(&AccessoryActive::acc_disconnected);
}

//acc_disconnected ------------------------------------------------
QSTATE_HANDLER_DEF(AccessoryActive, acc_disconnected, event) {
	if( relayEvent( event, false))
		return Q_SUPER(&QHsm::top);

	switch(  event->sig) {
	case ACC_SIG_DETACH:
		LOG("device detached ignored");
		break;
	case ACC_SIG_ATTACH:
		LOG("device attached");
		if(usb.devAttached()) {
				return Q_TRAN( &AccessoryActive::usb_settle);
		}
		return Q_HANDLED();
	case Q_INIT_SIG:
		LOG("wait for device");
		return Q_HANDLED();
	}
	return Q_SUPER(&QHsm::top);
}

//acc_connecting {settle,configure} ----------------------------

QSTATE_HANDLER_DEF(AccessoryActive, acc_connecting, event) {
	switch(  event->sig) {
	case ACC_SIG_DETACH:
		LOG("device detached");
		return Q_TRAN( &AccessoryActive::acc_disconnected);
	case ACC_SIG_ATTACH:
		LOG("device attached");
		if(usb.devAttached()) {
				return Q_TRAN( &AccessoryActive::usb_settle);
		}
		return Q_HANDLED();
	case Q_INIT_SIG:
		return Q_TRAN( &AccessoryActive::usb_settle);
	}
	return Q_SUPER(&QHsm::top);
}
QSTATE_HANDLER_DEF(AccessoryActive, usb_settle, event) {
	if( relayEvent( event, false))
			return Q_SUPER(&QHsm::top);

	switch(  event->sig) {
	case Q_INIT_SIG:
		usb.init();
		LOG("prepare to settle");
		timeE.postIn( this, MS2TICKS(200));
		return Q_HANDLED();
	case EVENT_SIG_TIMEOUT:
		LOG("to reset bus now");
		usb.busReset();
		usb.enInt( bmBUSEVENTIE);
		return Q_HANDLED();
	case ACC_SIG_BUSEVENT:
		LOG("wait for SOF");
		usb.enSOF();
		return Q_HANDLED();
	case ACC_SIG_SOF:
		LOG("configuring");
		return Q_TRAN( &AccessoryActive::usb_configure);
	}
	return Q_SUPER( &AccessoryActive::acc_connecting);
}

QSTATE_HANDLER_DEF(AccessoryActive, usb_configure, event) {
	if( relayEvent( event, false))
			return Q_SUPER(&QHsm::top);

	switch(  event->sig) {
	case Q_EXIT_SIG:
		timeE.disarm();
		break;
	case Q_INIT_SIG:
		timeE.postIn( this, MS2TICKS(20));
		return Q_HANDLED();
	case EVENT_SIG_TIMEOUT:
		LOG("get desc size");
		usb.init();
		if( usb.getDescSize()) {
			LOG("addressing");
			usb.setAddress();
		}
		LOG("connecting");
		if( connect())
			return Q_TRAN( &AccessoryActive::acc_connected);
		else
			return Q_HANDLED();
		break;
	}
	return Q_SUPER( &AccessoryActive::acc_connecting);
}
//acc_connected { usb_running } --------------------------------------------
QSTATE_HANDLER_DEF(AccessoryActive, acc_connected, event) {
	if( relayEvent( event))
			return Q_SUPER(&QHsm::top);

	switch(  event->sig) {
	case ACC_SIG_DETACH:
	case ACC_SIG_ATTACH:
		return Q_TRAN( &AccessoryActive::acc_disconnected);
	case EVENT_SIG_TIMEOUT:
		reqIn();
		return Q_HANDLED();
	case Q_INIT_SIG:
		LOG("connected");
		LOG("reporting SUBers");
		QF::reclaimSubs();
		reqIn();
		return Q_HANDLED();
	case Q_EXIT_SIG:
		timeE.disarm();
		usb.disInt(bmHXFRDNIE);
		QF::removeObserver(this);
		break;
	case Q_ENTRY_SIG:
		usb.enInt(bmHXFRDNIE);
		QF::setObserver(this);
		break;
	case 0: //QEP_EMPTY_SIG_
		break;
	}
	return Q_SUPER( &QHsm::top);
}

//end: state handlers .............................................






