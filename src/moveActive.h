#ifndef _MOVEACTIVE_H
#define _MOVEACTIVE_H

#include <qp/qp_port.h>

#undef __CLASS__
#define __CLASS__ MoveActive
class MoveActive : public QActive {

public:
  MoveActive();
  void start( uint8_t prio);
  void bspInit();

//state handlers
public:
  QSTATE_HANDLER(initial);
  QSTATE_HANDLER(run);

};

extern MoveActive moveActive;

#endif //_MOVEACTIVE_H
