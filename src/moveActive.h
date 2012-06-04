#ifndef _MOVEACTIVE_H
#define _MOVEACTIVE_H

#include <qp/qp_port.h>

class MoveActive : public QActive {
public:
  MoveActive();
  void start( uint8_t prio);
  void bspInit();

  bool lF;
  byte lS;
  bool rF;
  byte rS;

  void direct(bool forward, byte speed);
  void steer( bool left, byte speed);
  void accelerate( bool accel, byte speed);
  void brake( bool hard);
  void move(bool leftForward, byte leftSpeed, bool rightForward, byte rightSpeed);

//state handlers
public:
  QSTATE_HANDLER(initial);
  QSTATE_HANDLER(run);

};

extern MoveActive moveActive;

#endif //_MOVEACTIVE_H
