#ifndef _ACCESSORYACTIVE_H
#define _ACCESSORYACTIVE_H

#include <androidAccessory/AndroidAccessory.h>
#include <qp/qp_port.h>

#undef __CLASS__
#define __CLASS__ AccessoryActive
class AccessoryActive: public QActive {

	static AndroidAccessory acc;
public:
	//interrupt handler
	static void intHandler();

	AccessoryActive();
	void start( byte prio);
	void bspInit();


// state handlers
public:
  QSTATE_HANDLER(initial);
  QSTATE_HANDLER(run);

};

extern AccessoryActive accessoryActive;

#endif //_ACCESSORYACTIVE_H
