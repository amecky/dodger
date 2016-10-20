#include "CubeEmitter.h"

v2 CircleCubeEmitter::get(int index, int total) {
	if (index == 0) {
		return _position;
	}
	++_cnt;
	if (_cnt > _threshold) {
		_threshold += 4;
		++_ring;
		_cnt = 0;
		_offset = math::random(0.0f, DEGTORAD(45.0f));
	}
	float step = TWO_PI / _threshold;
	float angle = step * static_cast<float>(index % _threshold) + math::random(DEGTORAD(-15.0f), DEGTORAD(15.0f));
	float r = _radius * _ring;
	return _position + math::getRadialVelocity(angle, _radius * _ring);
}

v2 LineCubeEmitter::get(int index, int total) {
	v2 p = _position;
	if (_vertical) {
		p.x += index * _distance;
	}
	else {
		p.y += index * _distance;
	}
	return p;
}
