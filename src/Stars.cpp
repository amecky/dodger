#include "Stars.h"
#include "Constants.h"

Stars::Stars(GameContext* context) : _context(context) {
	_scale_path.add(0.0f, 0.1f);
	_scale_path.add(0.5f, 1.5f);
	_scale_path.add(0.75f, 0.75f);
	_scale_path.add(1.0f, 1.0f);
}


Stars::~Stars() {
}

void Stars::clear() {
	_stars.clear();
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
			float scale = 1.0f;
			_scale_path.get(s.timer / _context->settings->starTTL, &scale);
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
void Stars::add(const v2& pos,int count) {
	if (_stars.numObjects + count < 128) {
		if (count == 1) {
			ID id = _stars.add();
			Star& s = _stars.get(id);
			s.position = pos;
			s.timer = 0.0f;
			s.scale = v2(1, 1);
			s.texture = ds::math::buildTexture(0, 40, 24, 24);
			s.color = ds::Color(255, 180, 0, 255);
		}
		else {
			// spread out with radius = 20
			float step = TWO_PI / static_cast<float>(count);
			for (int i = 0; i < count; ++i) {
				ID id = _stars.add();
				Star& s = _stars.get(id);
				s.position.x = pos.x + 20.0f * cos(step * static_cast<float>(i));
				s.position.y = pos.y + 20.0f * sin(step * static_cast<float>(i));
				s.timer = 0.0f;
				s.scale = v2(1, 1);
				s.texture = ds::math::buildTexture(0, 40, 24, 24);
				s.color = ds::Color(255, 180, 0, 255);
			}
		}
	}
}

// ---------------------------------------
// render
// ---------------------------------------
void Stars::render() {
	for (int i = 0; i < _stars.numObjects; ++i) {
		ds::sprites::draw(_stars.objects[i]);
	}
}
