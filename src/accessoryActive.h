#ifndef _ACCESSORYACTIVE_H
#define _ACCESSORYACTIVE_H

#include <androidAccessory/AndroidAccessory.h>
#include <qp/qp_port.h>

class AccessoryActive: public QActive, public AndroidAccessory{
public:
	//interrupt handler
	static void IntHandler();
	void intHandler();

	AccessoryActive();
	void start( byte prio);
	void bspInit();
	/**
	 * recognize and relay event upstream, if enabled.
	 * otherwise ignore events
	 * return true is handled, false to continue
	 */
	bool relayEvent( const QEvent* event, bool enable= true);

// state handlers
public:
  QSTATE_HANDLER(initial);
  //acc_disconnected
  QSTATE_HANDLER(acc_disconnected);
  //acc_connecting {settle,configure}
  QSTATE_HANDLER(acc_connecting);
  QSTATE_HANDLER(usb_settle);
  QSTATE_HANDLER(usb_configure);
  //acc_connected { usb_running }
  QSTATE_HANDLER(acc_connected);
};

extern AccessoryActive accessoryActive;
#endif //_ACCESSORYACTIVE_H
