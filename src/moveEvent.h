/*
 * moveEvent.h
 *
 *  Created on: 2012-6-4
 *      Author: gen
 */

#ifndef MOVEEVENT_H_
#define MOVEEVENT_H_

#include <qp/qp_port.h>

#define MOVE_RAW_DATA_LEN 2
struct MoveRawEvent {
	struct QEvent e;
	char leftSpeed;
	char rightSpeed;
};

#define MOVE_STEER_DATA_LEN 1
struct MoveSteerEvent {
	struct QEvent e;
	char direction;
};

#define MOVE_ACCEL_DATA_LEN 1
struct MoveAccelEvent {
	struct QEvent e;
	char speed;
};

#define MOVE_BRAKE_DATA_LEN 1
struct MoveBrakeEvent {
	struct QEvent e;
	byte level;
};

#endif /* MOVEEVENT_H_ */
