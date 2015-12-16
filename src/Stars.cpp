#include "Stars.h"
#include "Constants.h"

Stars::Stars(GameContext* context) : _context(context) {
	_scale_path.add(0.0f, v2(0.1f));
	_scale_path.add(0.5f, v2(1.5f));
	_scale_path.add(0.75f, v2(0.75f));
	_scale_path.add(1.0f, v2(1.0f));
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
	StarArray::iterator it = _stars.begin();
	while (it != _stars.end()) {
		it->timer += dt;
		if (it->timer > _context->settings->starTTL) {
			it = _stars.remove(it->id);
		}
		else {
			_scale_path.get(it->timer / _context->settings->starTTL, &it->scale);
			++it;
		}
	}
}

// ---------------------------------------
// move towards player if in range
// ---------------------------------------
void Stars::move(const v2& target, float dt) {
	StarArray::iterator it = _stars.begin();
	while (it != _stars.end()) {
		v2 diff = target - it->position;
		if (sqr_length(diff) <  _context->settings->starMagnetRadius * _context->settings->starMagnetRadius) {
			v2 n = normalize(diff);
			n *= _context->settings->starSeekVelocity;
			it->position += n * dt;
		}
		++it;
	}
}

// ---------------------------------------
// pick up stars by player
// ---------------------------------------
int Stars::pickup(const v2& target, float radius) {
	int cnt = 0;
	StarArray::iterator it = _stars.begin();
	while (it != _stars.end()) {
		v2 diff = target - it->position;
		if (sqr_length(diff) < PLAYER_RADIUS * PLAYER_RADIUS) {
			++cnt;
			it = _stars.remove(it->id);
		}
		else {
			++it;
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
	StarArray::iterator it = _stars.begin();
	while (it != _stars.end()) {
		ds::sprites::draw(*it);
		++it;
	}
}
