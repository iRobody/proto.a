#ifndef _MOVEACTIVE_H
#define _MOVEACTIVE_H

#include <qp/qp_port.h>


#ifdef __CLASS__
#undef __CLASS__
#endif
#define __CLASS__ MoveActive

class MoveActive : public QActive {
  static QEvent const *eQueue[2];
  MoveActive* me;
public:
  //state handlers
  QSTATE_HANDLER(kick);
  QSTATE_HANDLER(ready);
public:
  MoveActive();
  void start( uint8_t prio);
  void bspInit();
};

extern MoveActive moveActive;

#endif //_MOVEACTIVE_H
