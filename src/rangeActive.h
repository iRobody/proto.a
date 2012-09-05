#ifndef _RANGEACTIVE_H
#define _RANGEACTIVE_H

#include <qp/qp_port.h>

class RangeActive : public QActive {
public:
  RangeActive();
  void start( uint8_t prio);
  void bspInit();

//state handlers
public:
  QSTATE_HANDLER(initial);
  QSTATE_HANDLER(run);

};

extern RangeActive rangeActive;

#endif //_RANGEACTIVE_H
