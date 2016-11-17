#include "ElasticBorder.h"
#include <renderer\SquareBuffer.h>
#include <resources\ResourceContainer.h>

const float Tension = 0.025f;
const float Dampening = 0.025f;
const float Spread = 0.15f;

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
	squares->end();
}

void AbstractBorder::splash(int index, float speed) {
	if (index >= 0 && index < _num) {
		_points[index].speed = speed;
	}
}

bool AbstractBorder::isInside(const v3& pos) const {
	return _box.contains(pos);
}


ElasticBorder::ElasticBorder() {
	_borders[0] = new BottomBorder(40.0f, 63);
	_borders[1] = new TopBorder(40.0f, 63);
	_borders[2] = new LeftBorder(40.0f, 34);
	_borders[3] = new RightBorder(40.0f, 34);
}

ElasticBorder::~ElasticBorder() {
	for (int i = 0; i < 4; ++i) {
		delete _borders[i];
	}
}

void ElasticBorder::tick(float dt) {
	for (int i = 0; i < 4; ++i) {
		_borders[i]->tick(dt);
	}
}
void ElasticBorder::render() {
	for (int i = 0; i < 4; ++i) {
		_borders[i]->render();
	}
}
void ElasticBorder::splash(const v3& pos, float speed) {
	for (int i = 0; i < 4; ++i) {
		if (_borders[i]->isInside(pos)) {
			_borders[i]->splash(_borders[i]->getIndex(pos), speed);
		}
	}
}