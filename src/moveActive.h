#ifndef _MOVEACTIVE_H
#define _MOVEACTIVE_H

#include <qp/qp_port.h>

class MoveActive : public QActive {
public:
  MoveActive();
  void start( uint8_t prio);
  void bspInit();

  signed char leftSpeed;
  signed char rightSpeed;

  void steer( signed char direction);
  void accelerate( signed char speed);
  void brake( unsigned char level);
  void move( signed char leftSpeed, signed char rightSpeed);

//state handlers
public:
  QSTATE_HANDLER(initial);
  QSTATE_HANDLER(run);

};

extern MoveActive moveActive;

#endif //_MOVEACTIVE_H
