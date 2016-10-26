#include "CubeEmitter.h"

// -----------------------------------------------
// circle cube emitter
// -----------------------------------------------
void CircleCubeEmitter::prepare(int total) {
	float x = math::random(150.0f, 900.0f);
	float y = math::random(150.0f, 600.0f);
	_position = v2(x, y);
	_threshold = 4;
	_ring = 1;
	_cnt = 0;
	_offset = math::random(0.0f, DEGTORAD(45.0f));
	_total = total;
}

EmitterData CircleCubeEmitter::next(int index) {
	EmitterData data;
	data.pos = _position;
	if (index > 0) {
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
		data.pos = _position + math::getRadialVelocity(angle, _radius * _ring);
		data.rotation = angle;
	}
	return data;
}

// -----------------------------------------------
// line cube emitter
// -----------------------------------------------
EmitterData LineCubeEmitter::next(int index) {
	EmitterData data;
	v2 p = _data.pos;
	if (_vertical) {
		p.x += index * _distance;
		data.rotation = DEGTORAD(90.0f);
	}
	else {
		p.y += index * _distance;
		data.rotation = 0.0f;
	}
	return data;
}

// -----------------------------------------------
// circle cube emitter
// -----------------------------------------------
void CornerCircleCubeEmitter::prepare(int total) {
	float x = math::random(150.0f, 900.0f);
	float y = math::random(150.0f, 600.0f);
	_position = v2(x, y);
	_threshold = 4;
	_ring = 1;
	_cnt = 0;
	_offset = math::random(0.0f, DEGTORAD(45.0f));
	_total = total;
	int r = 1;
	int c = 0;
	int t = 4;
	for (int i = 1; i < total; ++i) {
		++c;
		if (c > t) {
			t += 4;
			++r;
			c = 0;
		}
	}
	_outerRadius = r * _radius + 20.0f;
	float angle = DEGTORAD(45.0f);
	v2 p = v2(0, 0);
	bool left = math::chanceRoll(50);
	bool bottom = math::chanceRoll(50);
	if (!left && bottom) {
		angle += HALF_PI;
		p = v2(1600, 0);
	}
	else if (!left && !bottom) {
		angle += PI;
		p = v2(1600, 720);
	}
	else if (left && !bottom) {
		angle += PI;
		angle += HALF_PI;
		p = v2(0, 720);
	}
	_position = p + math::getRadialVelocity(angle, _outerRadius);
}

EmitterData CornerCircleCubeEmitter::next(int index) {
	EmitterData data;
	data.pos = _position;
	if (index > 0) {
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
		data.pos = _position + math::getRadialVelocity(angle, _radius * _ring);
		data.rotation = angle;
	}
	return data;
}
