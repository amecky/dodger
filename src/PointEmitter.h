#pragma once
#include <Vector.h>
#include <Point.h>

// ---------------------------------------------
// virtual point emitter
// ---------------------------------------------
class PointEmitter {

public:
	PointEmitter(int num);
	PointEmitter(int num, const p2i& gridPos);
	virtual ~PointEmitter();
	virtual v2 next(int index) = 0;
protected:
	int _num;
	p2i _gridPos;
};

// ---------------------------------------------
// random point emitter
// ---------------------------------------------
class RandomPointEmitter : public PointEmitter {

public:
	RandomPointEmitter(int num) : PointEmitter(num) {}
	RandomPointEmitter(int num, const p2i& gridPos) : PointEmitter(num, gridPos) {}
	virtual ~RandomPointEmitter() {}
	v2 next(int index);
};

// ---------------------------------------------
// vertical line emitter
// ---------------------------------------------
class VerticalLineEmitter : public PointEmitter {

public:
	VerticalLineEmitter(int num) : PointEmitter(num) {}
	VerticalLineEmitter(int num, const p2i& gridPos) : PointEmitter(num, gridPos) {}
	virtual ~VerticalLineEmitter() {}
	v2 next(int index);
};

// ---------------------------------------------
// horizontal line emitter
// ---------------------------------------------
class HorizontalLineEmitter : public PointEmitter {

public:
	HorizontalLineEmitter(int num) : PointEmitter(num) {}
	HorizontalLineEmitter(int num, const p2i& gridPos) : PointEmitter(num, gridPos) {}
	virtual ~HorizontalLineEmitter() {}
	v2 next(int index);
};