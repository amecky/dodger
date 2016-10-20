#pragma once
#include <core\math\math.h>

class CubeEmitter {

public:
	virtual void next() = 0;
	const v2& get() const {
		return _position;
	}
	virtual v2 get(int index, int total) = 0;
protected:
	v2 _position;
};

class TestCubeEmitter : public CubeEmitter {

public:
	void next() {
		_position = v2(512, 384);
	}

	v2 get(int index, int total) {
		return _position;
	}
};

class RandomCubeEmitter : public CubeEmitter {

public:

	void next() {
		float x = math::random(150.0f, 900.0f);
		float y = math::random(150.0f, 600.0f);
		_position = v2(x, y);
	}

	v2 get(int index, int total) {
		return _position;
	}
};

class CircleCubeEmitter : public CubeEmitter {

public:
	CircleCubeEmitter(float radius) : _radius(radius) {}

	void next() {
		float x = math::random(150.0f, 900.0f);
		float y = math::random(150.0f, 600.0f);
		_position = v2(x, y);
		_threshold = 4;
		_ring = 1;
		_cnt = 0;
		_offset = math::random(0.0f, DEGTORAD(45.0f));
	}

	v2 get(int index, int total);

private:
	float _radius;
	int _threshold;
	int _cnt;
	int _ring;
	float _offset;
};

class LineCubeEmitter : public CubeEmitter {

public:
	LineCubeEmitter(float distance) : _distance(distance), _vertical(false) {}

	void next() {
		float x = math::random(150.0f, 900.0f);
		float y = math::random(150.0f, 600.0f);
		_position = v2(x, y);
		_vertical = math::chanceRoll(50);
	}

	v2 get(int index, int total);

private:
	bool _vertical;
	float _distance;
};


