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

void AccessoryActive::intHandler() {

	byte irq = usb.IntHandler();

	if( irq & bmBUSEVENTIRQ) {
		usb.disInt(bmBUSEVENTIE);
		postFIFO(&busEvent);
		return;
	}
	if( irq & bmCONDETIRQ) {
		if( usb.devAttached())
			postFIFO(&attEvent);
		else
			postFIFO(&detEvent);
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
			buff = (char*)&(pEvent->data);
			if( pEvent->length)
				if(!readFIFO( buff, pEvent->length))
					break;
			pEvent->type = 0;
			QF::publish(pEvent);
			return;
		}
		if( pEvent)
			QF::gc( pEvent);
	}

	if( irq & bmHXFRDNIRQ) {
		USER_LED_TOGGLE();
		reqIn();
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

//begin: state handlers ............................................
QSTATE_HANDLER_DEF(AccessoryActive, initial, event) {
	subscribe(EVENT_CH_ACC_C);
	return Q_TRAN(&AccessoryActive::acc_disconnected);
}

//acc_disconnected ------------------------------------------------
QSTATE_HANDLER_DEF(AccessoryActive, acc_disconnected, event) {
	switch(  event->sig) {
	case ACC_SIG_DETACH:
		LOG("device detached ignored");
		break;
	case ACC_SIG_ATTACH:
		LOG("device attached");
		return Q_TRAN( &AccessoryActive::acc_connecting);
	case Q_INIT_SIG:
		if(usb.devAttached()) {
			return Q_TRAN( &AccessoryActive::usb_settle);
		}
		LOG("wait for device");
		return Q_HANDLED();
	}
	return Q_SUPER(&QHsm::top);
}

//acc_connecting {settle,configure} ----------------------------
static QTimeEvt timeE = QTimeEvt(EVENT_SIG_TIMEOUT);

QSTATE_HANDLER_DEF(AccessoryActive, acc_connecting, event) {
	switch(  event->sig) {
	case ACC_SIG_DETACH:
		LOG("device detached");
		return Q_TRAN( &AccessoryActive::acc_disconnected);
	case ACC_SIG_ATTACH:
		LOG("device attached");
		return Q_TRAN( &AccessoryActive::acc_connecting);
	case Q_INIT_SIG:
		if(usb.devAttached()) {
			return Q_TRAN( &AccessoryActive::usb_settle);
		}
		return Q_TRAN( &AccessoryActive::acc_disconnected);
	}
	return Q_SUPER(&QHsm::top);
}
QSTATE_HANDLER_DEF(AccessoryActive, usb_settle, event) {
	switch(  event->sig) {
	case Q_INIT_SIG:
		usb.init();
		LOG("prepare to settle");
		timeE.postIn( this, TIME2TICKS(200));
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
	switch(  event->sig) {
	case Q_EXIT_SIG:
		timeE.disarm();
		break;
	case Q_INIT_SIG:
		timeE.postIn( this, TIME2TICKS(20));
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
	switch(  event->sig) {

	case Q_INIT_SIG:
		LOG("connected");
		reqIn();
		usb.enInt(bmHXFRDNIE);
		return Q_HANDLED();
	case ACC_SIG_DETACH:
	case ACC_SIG_ATTACH:
		return Q_TRAN( &AccessoryActive::acc_disconnected);
	case Q_EXIT_SIG:
		usb.disInt(bmHXFRDNIE);
		QF::removeObserver(this);
		break;
	case Q_ENTRY_SIG:
		QF::setObserver(this);
		break;
	case 0: //QEP_EMPTY_SIG_
		break;
	default:
		//relay event
		if(!(event->type & EVENT_TYPE_RELAY))
			break;
		write( (char*)&(event->type), event->length + EVENT_HEADER_SIZE);
		return Q_HANDLED();
	}
	return Q_SUPER( &QHsm::top);
}

//end: state handlers .............................................






