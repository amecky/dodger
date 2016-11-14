#include "Shapes.h"

struct ShapeDefinition {

	float velocity;
	float radius;
	int pieces;
	float radialVelocity;
	ds::Texture lineTexture;

	ShapeDefinition(int p, float r, float v, float rv, const ds::Texture& lt) : pieces(p), radius(r), velocity(v) , radialVelocity(rv) , lineTexture(lt) {}
};

const ShapeDefinition DEFINITIONS[] = {
	{ 11, 150.0f,  50.0f, 0.2f , math::buildTexture(0, 510, 100, 40 )},
	{  9, 110.0f, 100.0f, 0.4f , math::buildTexture(0, 510, 100, 40) },
	{  7,  70.0f, 150.0f, 0.6f , math::buildTexture(0, 510, 100, 40) },
	{  5,  40.0f, 200.0f, 0.8f , math::buildTexture(0, 510, 100, 40) }
};

Shapes::Shapes() {
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
	shape.velocity = math::getRadialVelocity(math::random(0.0f, TWO_PI), def.velocity);
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

float Shapes::getRadius(const Shape & s, int index) {
	int idx = index;
	if (idx < 0) {
		idx = s.num - 1;
	}
	return s.points[idx].radius + sin(s.points[idx].timer) * s.radius * 0.1f;
}

void Shapes::render() {
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	for (uint32_t i = 0; i < _shapes.size(); ++i) {
		const Shape& shape = _shapes[i];
		const ShapeDefinition& def = DEFINITIONS[shape.type];
		for (int j = 0; j < shape.num; ++j) {
			v2 p = shape.position;
			v2 n = shape.position;			
			float pr = getRadius(shape, j);
			float pa = getAngle(shape, j);
			float nr = getRadius(shape, j - 1);
			float na = getAngle(shape, j - 1);
			math::addRadial(p, pr, pa);
			if (j == 0) {
				math::addRadial(n, nr, na);
			}
			else {
				math::addRadial(n, nr, na);
			}
			sprites->drawLine(n,p, def.lineTexture,ds::Color(192,0,0,255));
		}
		for (int j = 0; j < shape.num; ++j) {
			v2 p = shape.position;
			float pr = getRadius(shape, j);
			float pa = getAngle(shape, j);
			math::addRadial(p, pr, pa);
			sprites->draw(p, math::buildTexture(0, 460, 16, 16), pa,v2(1,1),ds::Color(192,64,0,255));
		}
	}
}
