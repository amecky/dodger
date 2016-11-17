#include "Borders.h"
#include <renderer\SquareBuffer.h>
#include <resources\ResourceContainer.h>

Borders::Borders() {
	_total = MAX_X_LP + MAX_Y_LP;
	_squareBuffer = ds::res::find("Squares", ds::ResourceType::SQUAREBUFFER);
	for (int i = 0; i < _total; ++i) {
		_points[i].pos = v2(20 + i * 40, 22);
		_points[i].offset = 0.0f;
		_points[i].timer = math::random(0.0f, TWO_PI);
	}	
}


Borders::~Borders() {
}

// ---------------------------------------------------
// tick
// ---------------------------------------------------
void Borders::tick(float dt) {
	ZoneTracker("Borders::tick");
	for (int i = 1; i < _total - 1; ++i) {
		_points[i].timer += dt;
		if (_points[i].timer >= TWO_PI) {
			_points[i].timer = math::random(0.0f, TWO_PI);
		}
		_points[i].offset = sin(_points[i].timer * 15.0f) * 2;
	}
	_points[MAX_X_LP - 1].offset = 0.0f;
	_points[MAX_X_LP].offset = 0.0f;
}

// ---------------------------------------------------
// render
// ---------------------------------------------------
void Borders::render() {
	ZoneTracker("Borders::render");
	ds::SquareBuffer* squares = ds::res::getSquareBuffer(_squareBuffer);
	squares->begin();
	v3 p[4];
	// top and down
	float yp = 22.0f;
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < MAX_X_LP - 1; ++i) {
			v2 f = v2(20.0f + i * 40.0f,_points[i].offset + yp);
			v2 s = v2(20.0f + (i + 1)* 40.0f, _points[i+1].offset + yp);
			p[0] = v3(f.x, f.y + 10.0f, 0.0f);
			p[1] = v3(s.x, s.y + 10.0f, 0.0f);
			p[2] = v3(s.x, s.y - 10.0f, 0.0f);
			p[3] = v3(f.x, f.y - 10.0f, 0.0f);
			squares->draw(p, math::buildTexture(92, 10, 40, 20), ds::Color(43, 221, 237, 255));
		}
		yp = 680.0f;
	}
	// left and right
	float xp = 22.0f;
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < MAX_Y_LP - 1; ++i) {
			v2 f = v2(_points[i + MAX_X_LP].offset + xp, 30.0f + i * 40.0f);
			v2 s = v2(_points[i + MAX_X_LP + 1].offset + xp, 30.0f + (i + 1)* 40.0f);
			p[0] = v3(s.x - 10.0f, s.y, 0.0f);
			p[1] = v3(s.x + 10.0f, s.y, 0.0f);
			p[2] = v3(f.x + 10.0f, f.y, 0.0f);
			p[3] = v3(f.x - 10.0f, f.y, 0.0f);
			squares->draw(p, math::buildTexture(0, 260, 20, 40), ds::Color(43, 221, 237, 255));
		}
		xp = 1255.0f;
	}
	// corner dots
	v3 dp[] = { v3(-10, 10, 0), v3(10, 10, 0), v3(10, -10, 0), v3(-10, -10, 0) };
	v3 cp[] = { v3(20, 20, 0), v3(20, 680, 0), v3(1250, 680, 0), v3(1250, 20, 0) };
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			p[j] = cp[i] + dp[j];
		}
		squares->draw(p, math::buildTexture(40, 120, 20, 20), ds::Color(43, 221, 237, 255));
	}
	squares->end();
}
