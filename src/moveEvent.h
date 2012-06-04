/*
 * moveEvent.h
 *
 *  Created on: 2012-6-4
 *      Author: gen
 */

#ifndef MOVEEVENT_H_
#define MOVEEVENT_H_

#include <qp/qp_port.h>

struct MoveDirectEvent {
	struct QEvent e;
	byte forward;
	byte speed;
};

struct MoveSteerEvent {
	struct QEvent e;
	byte left;
	byte speed;
};

struct MoveAccelEvent {
	struct QEvent e;
	byte accel;
	byte speed;
};

struct MoveBrakeEvent {
	struct QEvent e;
	byte hard;
};

struct MoveRawEvent {
	struct QEvent e;
	byte lF;
	byte lS;
	byte rF;
	byte rS;
};


#endif /* MOVEEVENT_H_ */
