#include "Stars.h"
#include "Constants.h"

Stars::Stars(GameContext* context) : _context(context) {
}


Stars::~Stars() {
}

// ---------------------------------------
// tick
// ---------------------------------------
void Stars::tick(float dt) {
	for (int i = 0; i < _stars.numObjects; ++i) {
		Star& s = _stars.objects[i];
		s.timer += dt;
		if (s.timer > _context->settings->starTTL) {
			_stars.remove(s.id);
		}
		else {
			float scale = _context->settings->starBaseScale + sin(s.timer *  _context->settings->starScaleAmplitude) *  _context->settings->starScaleFactor;
			s.scale.x = scale;
			s.scale.y = scale;
		}
	}
}

// ---------------------------------------
// move towards player if in range
// ---------------------------------------
void Stars::move(const v2& target, float dt) {
	for (int i = 0; i < _stars.numObjects; ++i) {
		Star& s = _stars.objects[i];
		v2 diff = target - s.position;
		if (sqr_length(diff) <  _context->settings->starMagnetRadius * _context->settings->starMagnetRadius) {
			v2 n = normalize(diff);
			n *= _context->settings->starSeekVelocity;
			s.position += n * dt;
		}
	}
}

// ---------------------------------------
// pick up stars by player
// ---------------------------------------
int Stars::pickup(const v2& target, float radius) {
	int cnt = 0;
	for (int i = 0; i < _stars.numObjects; ++i) {
		Star& s = _stars.objects[i];
		v2 diff = target - s.position;
		if (sqr_length(diff) < PLAYER_RADIUS * PLAYER_RADIUS) {
			++cnt;
			_stars.remove(s.id);
		}
	}
	return cnt;
}

// ---------------------------------------
// add new star
// ---------------------------------------
void Stars::add(const v2& pos) {
	if (_stars.numObjects + 1 < 128) {
		ID id = _stars.add();
		Star& s = _stars.get(id);
		s.position = pos;
		s.timer = 0.0f;
		s.scale = v2(1, 1);
	}
}

// ---------------------------------------
// render
// ---------------------------------------
void Stars::render() {
	for (int i = 0; i < _stars.numObjects; ++i) {
		const Star& s = _stars.objects[i];
		ds::sprites::draw(s.position, ds::math::buildTexture(0, 40, 24, 24), 0.0f, s.scale.x, s.scale.y,ds::Color(255,180,0,255));
	}
}
