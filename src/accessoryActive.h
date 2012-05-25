#ifndef _ACCESSORYACTIVE_H
#define _ACCESSORYACTIVE_H

#include <androidAccessory/AndroidAccessory.h>
#include <qp/qp_port.h>

#ifdef __CLASS__
#undef __CLASS__
#endif
#define __CLASS__ AccessoryActive

class AccessoryActive: public QActive {
  static AndroidAccessory acc;
  static QEvent const *eQueue[2];
  AccessoryActive* me;
public:
  //state handlers
  QSTATE_HANDLER(kick);
  QSTATE_HANDLER(ready);

public:
  AccessoryActive();
  void start( byte prio);
  //int handler
  static void intHandler();  
  void bspInit();
};

extern AccessoryActive accessoryActive;

#endif //_ACCESSORYACTIVE_H
