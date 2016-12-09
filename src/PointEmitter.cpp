#include "PointEmitter.h"
#include <core\math\math.h>

PointEmitter::PointEmitter(int num) : _num(num) , _gridPos(0,0) {
}

PointEmitter::PointEmitter(int num, const p2i& gridPos) : _num(num) , _gridPos(gridPos) {
}

PointEmitter::~PointEmitter() {
}


v2 RandomPointEmitter::next(int index) {
	int sx = math::random(2, 38);
	int sy = math::random(2, 19);
	return v2(49 + sx * 30, 45 + sy * 30);
}

v2 VerticalLineEmitter::next(int index) {
	int sx = _gridPos.x + index;
	int sy = _gridPos.y;
	return v2(49 + sx * 30, 45 + sy * 30);
}

v2 HorizontalLineEmitter::next(int index) {
	int sx = _gridPos.x;
	int sy = _gridPos.y + index;
	return v2(49 + sx * 30, 45 + sy * 30);
}