#pragma once
#include <core\math\math.h>

const ds::Rect WORLD_RECT = ds::Rect(25, 20, 1230, 640);

struct EmitterData {
	v2 pos;
	float rotation;

	EmitterData() : pos(0.0f), rotation(0.0f) {}
};
// -----------------------------------------------
// cube emitter
// -----------------------------------------------
class CubeEmitter {

public:
	virtual void prepare(int total) = 0;
	virtual EmitterData next(int index = 0) = 0;
protected:
	EmitterData _data;
	uint32_t _total;
};

// -----------------------------------------------
// test cube emitter
// -----------------------------------------------
class TestCubeEmitter : public CubeEmitter {

public:
	void prepare(int total) {
		_total = total;
		_data.pos = v2(640, 360);
	}
	EmitterData next(int index = 0) {
		return _data;
	}
};

// -----------------------------------------------
// random cube emitter
// -----------------------------------------------
class RandomCubeEmitter : public CubeEmitter {

public:
	void prepare(int total) {
		_total = total;
		float x = math::random(150.0f, 900.0f);
		float y = math::random(150.0f, 600.0f);
		_data.pos = v2(x, y);
	}
	EmitterData next(int index = 0) {
		return _data;
	}
};

// -----------------------------------------------
// circle cube emitter
// -----------------------------------------------
class CircleCubeEmitter : public CubeEmitter {

public:
	CircleCubeEmitter(float radius) : _radius(radius) {}
	void prepare(int total);
	EmitterData next(int index = 0);
private:
	v2 _position;
	float _radius;
	int _threshold;
	int _cnt;
	int _ring;
	float _offset;
};

// -----------------------------------------------
// line cube emitter
// -----------------------------------------------
class LineCubeEmitter : public CubeEmitter {

public:
	LineCubeEmitter(float distance) : _distance(distance), _vertical(false) {}
	void prepare(int total) {
		_total = total;
		float x = math::random(150.0f, 900.0f);
		float y = math::random(150.0f, 600.0f);
		_data.pos = v2(x, y);
		_vertical = math::chanceRoll(50);
	}
	EmitterData next(int index = 0);
	void next() {
		float x = math::random(150.0f, 900.0f);
		float y = math::random(150.0f, 600.0f);
		_data.pos = v2(x, y);
		_vertical = math::chanceRoll(50);
	}
private:
	bool _vertical;
	float _distance;
};

// -----------------------------------------------
// corner cube emitter
// -----------------------------------------------
class CornerCircleCubeEmitter : public CubeEmitter {

public:
	CornerCircleCubeEmitter(float radius) : _radius(radius) {}
	void prepare(int total);
	EmitterData next(int index = 0);
private:
	float _outerRadius;
	v2 _position;
	float _radius;
	int _threshold;
	int _cnt;
	int _ring;
	float _offset;
};


