#include "Shapes.h"
#include <resources\ResourceContainer.h>

struct ShapeDefinition {

	float velocity;
	float radius;
	int pieces;
	float radialVelocity;
	ds::Texture lineTexture;
	float thickness;
	float radiusVariance;

	ShapeDefinition(int p, float r, float v, float rv, const ds::Texture& lt, float t,float rav) : pieces(p), radius(r), velocity(v) , radialVelocity(rv) , lineTexture(lt) , thickness(t) , radiusVariance(rav) {}
};

const ShapeDefinition DEFINITIONS[] = {
	{ 11, 150.0f,  50.0f, 0.2f , math::buildTexture(0, 510, 100, 40), 20.0f, 8.0f},
	{  9, 110.0f, 100.0f, 0.4f , math::buildTexture(0, 510, 100, 40), 15.0f, 6.0f},
	{  7,  70.0f, 150.0f, 0.6f , math::buildTexture(0, 510, 100, 40), 14.0f, 4.0f},
	{  6,  40.0f, 200.0f, 0.8f , math::buildTexture(0, 510, 100, 40), 12.0f, 4.0f}
};

Shapes::Shapes() {
	_squareBuffer = ds::res::find("Squares", ds::ResourceType::SQUAREBUFFER);
}


Shapes::~Shapes() {
}

void Shapes::tick(float dt) {
	for (uint32_t i = 0; i < _shapes.size(); ++i) {
		Shape& shape = _shapes[i];
		const ShapeDefinition& def = DEFINITIONS[shape.type];
		shape.position += shape.velocity * dt;
		if (shape.position.x < (shape.radius)) {
			shape.velocity.x *= -1.0f;
		}
		if (shape.position.x > (1280 - shape.radius)) {
			shape.velocity.x *= -1.0f;
		}
		if (shape.position.y < (shape.radius)) {
			shape.velocity.y *= -1.0f;
		}
		if (shape.position.y >(720 - shape.radius)) {
			shape.velocity.y *= -1.0f;
		}
		for (int j = 0; j < shape.num; ++j) {
			ShapePoint& point = shape.points[j];
			point.timer += dt;
		}
		shape.angle += dt * def.radialVelocity;
	}
}

void Shapes::create(const v2& pos, int type) {
	const ShapeDefinition& def = DEFINITIONS[type];
	float step = TWO_PI / static_cast<float>(def.pieces);
	Shape shape;
	shape.num = def.pieces;
	shape.position = pos;
	shape.timer = 0.0f;
	shape.velocity = v2(0, 0);// math::getRadialVelocity(math::random(0.0f, TWO_PI), def.velocity);
	shape.radius = def.radius;
	shape.type = type;
	for (int i = 0; i < def.pieces; ++i) {
		ShapePoint& point = shape.points[i];
		point.angle = i * step + math::random(-DEGTORAD(4.0f),DEGTORAD(4.0f));
		point.radius = math::randomRange(def.radius, def.radius*0.01f);
		point.timer = math::random(0.0f, TWO_PI);
	}
	_shapes.push_back(shape);
}

float Shapes::getAngle(const Shape& s, int index) {
	int idx = index;
	if (idx < 0) {
		idx = s.num - 1;
	}
	return s.angle + s.points[idx].angle + sin(s.points[idx].timer) * DEGTORAD(4.0f);
}

float Shapes::getRadius(const Shape & s, int index, bool inner) {
	int idx = index;
	if (idx < 0) {
		idx = s.num - 1;
	}
	const ShapeDefinition& def = DEFINITIONS[s.type];
	float r = 0.0f;
	if (inner) {
		r = -def.thickness;// -sin(s.points[idx].timer) * 2.0f;
	}
	else {
		r = def.thickness + sin(s.points[idx].timer) * def.radiusVariance;
	}
	return s.points[idx].radius + r;// +sin(s.points[idx].timer) * 0.1f;
}

void Shapes::render() {
	ds::SquareBuffer* squares = ds::res::getSquareBuffer(_squareBuffer);
	squares->begin();
	v2 p[4];
	v3 np[4];
	//ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	for (uint32_t i = 0; i < _shapes.size(); ++i) {
		const Shape& shape = _shapes[i];
		const ShapeDefinition& def = DEFINITIONS[shape.type];
		for (int j = 0; j < shape.num; ++j) {
			v2 fp = shape.position;
			v2 sp = shape.position;			
			float pa = getAngle(shape, j);
			float na = getAngle(shape, j - 1);
			p[0] = fp;
			p[1] = sp;
			p[2] = sp;
			p[3] = fp;

			math::addRadial(p[0], getRadius(shape, j, false), pa);
			math::addRadial(p[1], getRadius(shape, j - 1, false), na);
			math::addRadial(p[2], getRadius(shape, j - 1, true), na);
			math::addRadial(p[3], getRadius(shape, j, true), pa);

			for (int k = 0; k < 4; ++k) {
				np[k] = p[k];
			}
			squares->draw(np, def.lineTexture,ds::Color(192,0,0,255));
		}
		/*
		for (int j = 0; j < shape.num; ++j) {
			v2 p = shape.position;
			float pr = getRadius(shape, j);
			float pa = getAngle(shape, j);
			math::addRadial(p, pr, pa);
			sprites->draw(p, math::buildTexture(0, 460, 16, 16), pa,v2(1,1),ds::Color(192,32,0,255));
		}
		*/
	}
	squares->end();
}
