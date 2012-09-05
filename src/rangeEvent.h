#ifndef RANGEEVENT_H_
#define RANGEEVENT_H_

#include <qp/qp_port.h>

#define RANGE_EDGE_DATA_LEN 4

struct RangeEdgeEvent {
	struct QEvent e;
	/**
	 * edge position bit-mask as:
	 * 7bit|6bit|5bit|4bit|frontLeft|frontRight|rearLeft|rearRight
	 */
	byte frontLeft;
	byte frontRight;
	byte rearLeft;
	byte rearRight;
};

#endif /* RANGEEVENT_H_ */
