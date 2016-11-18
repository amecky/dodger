#include "ElasticBorder.h"
#include <renderer\SquareBuffer.h>
#include <resources\ResourceContainer.h>

const float Tension = 0.025f;
const float Dampening = 0.025f;
const float Spread = 0.15f;

// ---------------------------------------------------
//
// AbstractBorder
//
// ---------------------------------------------------
AbstractBorder::AbstractBorder(float target, int num, const ds::Texture texture, const ds::AABBox& box) 
	: _target(target) , _num(num) , _texture(texture) , _box(box) {
	_squareBuffer = ds::res::find("Squares", ds::ResourceType::SQUAREBUFFER);
	_points = new LinePoint[num];
	lDeltas = new float[num];
	rDeltas = new float[num];
	for (int i = 0; i < num; ++i) {
		_points[i].height = 40.0f;
		_points[i].targetHeight = 40.0f;
		_points[i].speed = 0.0f;
	}	
}


AbstractBorder::~AbstractBorder() {
	delete[] rDeltas;
	delete[] lDeltas;
	delete[] _points;
}

// ---------------------------------------------------
// tick
// ---------------------------------------------------
void AbstractBorder::tick(float dt) {
	ZoneTracker("Borders::tick");
	for (int i = 1; i < _num - 1; ++i) {
		LinePoint& lp = _points[i];
		float x = lp.targetHeight - lp.height;
		lp.speed += Tension * x - lp.speed * Dampening;
		lp.height += lp.speed;
	}

	// do some passes where columns pull on their neighbours
	for (int j = 0; j < 2; j++)	{
		for (int i = 0; i < _num; i++) {
			if (i > 0) {
				lDeltas[i] = Spread * (_points[i].height - _points[i - 1].height);
				_points[i - 1].speed += lDeltas[i];
			}
			if (i < _num - 1) {
				rDeltas[i] = Spread * (_points[i].height - _points[i + 1].height);
				_points[i + 1].speed += rDeltas[i];
			}
		}

		for (int i = 0; i < _num; i++) {
			if (i > 0) {
				_points[i - 1].height += lDeltas[i];
			}
			if (i < _num - 1) {
				_points[i + 1].height += rDeltas[i];
			}
		}
	}
	// fix corner points
	_points[0].height = _points[0].targetHeight;
	_points[_num - 1].height = _points[_num - 1].targetHeight;
}

// ---------------------------------------------------
// render
// ---------------------------------------------------
void AbstractBorder::render() {
	ZoneTracker("Borders::render");
	ds::SquareBuffer* squares = ds::res::getSquareBuffer(_squareBuffer);
	squares->begin();
	v3 p[4];
	for (int i = 0; i < _num - 1; ++i) {
		buildColumn(i, p);
		squares->draw(p, _texture, ds::Color(43, 221, 237, 255));
	}
	// corner dots
	v3 dp[] = { v3(-10, 10, 0), v3(10, 10, 0), v3(10, -10, 0), v3(-10, -10, 0) };
	v3 cp[] = { v3(35, 35, 0), v3(35, 665, 0), v3(1235, 665, 0), v3(1235, 35, 0) };
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			p[j] = cp[i] + dp[j];
		}
		squares->draw(p, math::buildTexture(40, 120, 20, 20), ds::Color(43, 221, 237, 255));
	}
	squares->end();
}

// ---------------------------------------------------
// splash
// ---------------------------------------------------
void AbstractBorder::splash(int index, float speed) {
	if (index >= 0 && index < _num) {
		_points[index].speed = speed;
	}
}

// ---------------------------------------------------
// is inside
// ---------------------------------------------------
bool AbstractBorder::isInside(const v3& pos) const {
	return _box.contains(pos);
}

bool AbstractBorder::collides(const ds::Sphere& s) const {
	return _box.overlaps(s);
}

// ---------------------------------------------------
//
// ElasticBorder
//
// ---------------------------------------------------
ElasticBorder::ElasticBorder() {
	_borders[0] = new BottomBorder(40.0f, 61);
	_borders[1] = new TopBorder(40.0f, 61);
	_borders[2] = new LeftBorder(40.0f, 33);
	_borders[3] = new RightBorder(40.0f, 33);
}

ElasticBorder::~ElasticBorder() {
	for (int i = 0; i < 4; ++i) {
		delete _borders[i];
	}
}

// ---------------------------------------------------
// tick
// ---------------------------------------------------
void ElasticBorder::tick(float dt) {
	for (int i = 0; i < 4; ++i) {
		_borders[i]->tick(dt);
	}
}

// ---------------------------------------------------
// render
// ---------------------------------------------------
void ElasticBorder::render() {
	for (int i = 0; i < 4; ++i) {
		_borders[i]->render();
	}
}

// ---------------------------------------------------
// splash
// ---------------------------------------------------
void ElasticBorder::splash(const v3& pos, float speed) {
	for (int i = 0; i < 4; ++i) {
		if (_borders[i]->isInside(pos)) {
			_borders[i]->splash(_borders[i]->getIndex(pos), speed);
		}
	}
}

void ElasticBorder::splash(const v3& pos, float radius, float speed) {
	ds::Sphere sp(pos, radius);
	for (int i = 0; i < 4; ++i) {
		if (_borders[i]->collides(sp)) {
			_borders[i]->splash(_borders[i]->getIndex(pos), speed);
		}
	}
}

// ---------------------------------------------------
//
// BottomBorder
//
// ---------------------------------------------------
BottomBorder::BottomBorder(float target, int num) 
	: AbstractBorder(target, num, math::buildTexture(0, 302, 36, 10), ds::AABBox(v3(640, 20, 0), v3(1280, 40, 0))) {
}

void BottomBorder::buildColumn(int index, v3* positions) {
	v2 f = v2(30.0f + index * 20.0f, _points[index].height);
	v2 s = v2(30.0f + (index + 1)* 20.0f, _points[index + 1].height);
	positions[0] = v3(f.x, f.y, 0.0f);
	positions[1] = v3(s.x, s.y, 0.0f);
	positions[2] = v3(s.x, s.y - 10.0f, 0.0f);
	positions[3] = v3(f.x, f.y - 10.0f, 0.0f);
}

int BottomBorder::getIndex(const v3& pos) const {
	return (pos.x - 20) / 20;
}

// ---------------------------------------------------
//
// TopBorder
//
// ---------------------------------------------------
TopBorder::TopBorder(float target, int num) 
	: AbstractBorder(target, num, math::buildTexture(30, 342, 36, 10), ds::AABBox(v3(640, 700, 0), v3(1280, 40, 0))) {
}

void TopBorder::buildColumn(int index, v3* positions) {
	v2 f = v2(30.0f + index * 20.0f, _points[index].height);
	v2 s = v2(30.0f + (index + 1)* 20.0f, _points[index + 1].height);
	positions[0] = v3(f.x, 700.0f - f.y + 10.0f, 0.0f);
	positions[1] = v3(s.x, 700.0f - s.y + 10.0f, 0.0f);
	positions[2] = v3(s.x, 700.0f - s.y, 0.0f);
	positions[3] = v3(f.x, 700.0f - f.y, 0.0f);
}

int TopBorder::getIndex(const v3& pos) const {
	return (pos.x - 20) / 20;
}

// ---------------------------------------------------
//
// LeftBorder
//
// ---------------------------------------------------
LeftBorder::LeftBorder(float target, int num) 
	: AbstractBorder(target, num, math::buildTexture(40, 330, 10, 36), ds::AABBox(v3(20, 360, 0), v3(20, 720, 0))) {
}

void LeftBorder::buildColumn(int index, v3* positions) {
	v2 f = v2(_points[index].height, 30.0f + index * 20.0f);
	v2 s = v2(_points[index + 1].height, 30.0f + (index + 1)* 20.0f);
	positions[0] = v3(s.x - 10.0f, s.y, 0.0f);
	positions[1] = v3(s.x, s.y, 0.0f);
	positions[2] = v3(f.x, f.y, 0.0f);
	positions[3] = v3(f.x - 10.0f, f.y, 0.0f);
}

int LeftBorder::getIndex(const v3& pos) const {
	return (pos.y - 20) / 20;
}

// ---------------------------------------------------
//
// RightBorder
//
// ---------------------------------------------------
RightBorder::RightBorder(float target, int num) 
	: AbstractBorder(target, num, math::buildTexture(40, 340, 10, 36), ds::AABBox(v3(1260, 360, 0), v3(20, 720, 0))) {
}

void RightBorder::buildColumn(int index, v3* positions) {
	v2 f = v2(_points[index].height, 30.0f + index * 20.0f);
	v2 s = v2(_points[index + 1].height, 30.0f + (index + 1)* 20.0f);
	positions[0] = v3(1270.0f - s.x, s.y, 0.0f);
	positions[1] = v3(1270.0f - s.x + 10.0f, s.y, 0.0f);
	positions[2] = v3(1270.0f - f.x + 10.0f, f.y, 0.0f);
	positions[3] = v3(1270.0f - f.x, f.y, 0.0f);
}

int RightBorder::getIndex(const v3& pos) const {
	return (pos.y - 20) / 20;
}