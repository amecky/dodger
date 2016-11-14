#pragma once
#include "..\GameContext.h"


struct ShapePoint {
	float radius;
	float angle;
	float timer;
};

struct Shape {
	ShapePoint points[16];
	int num;
	v2 position;
	float angle;
	v2 velocity;
	float timer;
	float radius;
	int type;
};

class Shapes {

public:
	Shapes();
	~Shapes();
	void tick(float dt);
	void create(const v2& pos, int type);
	void render();
private:
	float getAngle(const Shape& s, int index);
	float getRadius(const Shape& s, int index);
	ds::Array<Shape> _shapes;
};

