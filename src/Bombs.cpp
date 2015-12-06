#include "Bombs.h"


Bombs::Bombs(GameContext* context) : _context(context) {
	float step = TWO_PI / 36.0f;
	for (int i = 0; i < 36; ++i) {
		_cells[i] = 0.9f + sin(step * static_cast<float>(i) * 12.0f) * 0.4f;
	}
}


Bombs::~Bombs() {
}

// ---------------------------------------
// create
// ---------------------------------------
void Bombs::create() {
	ID id = _bombs.add();
	Bomb& b = _bombs.get(id);
	b.position.x = ds::math::random(100.0f, 1100.0f);
	b.position.y = ds::math::random(100.0f, 620.0f);
	float angle = ds::math::random(0.0f, TWO_PI);
	float v = ds::math::random(30.0f, 50.0f);
	b.velocity = ds::vector::getRadialVelocity(angle, v);
	b.scale = v2(1, 1);
	b.state = Bomb::BS_ACTIVE;
	b.timer = 0.0f;
	b.color = ds::Color(192, 128, 0, 255);
}

// ---------------------------------------
// grab bomb
// ---------------------------------------
bool Bombs::grab(const v2& pos, float radius, ID* id) {
	for (int i = 0; i < _bombs.numObjects; ++i) {
		Bomb& bomb = _bombs.objects[i];
		if (bomb.state == Bomb::BS_ACTIVE) {
			if (ds::math::checkCircleIntersection(_context->world_pos, radius, bomb.position, 20.0f)) {
				bomb.state = Bomb::BS_FOLLOWING;
				bomb.color = ds::Color(0, 192, 32, 255);
				*id = bomb.id;
				return true;
			}
		}
	}
	return false;
}

// ---------------------------------------
// contains
// ---------------------------------------
bool Bombs::contains(ID id) const {
	return _bombs.contains(id);
}

// ---------------------------------------
// follow target
// ---------------------------------------
void Bombs::follow(ID id, const v2& target) {
	if (_bombs.contains(id)) {
		Bomb& bomb = _bombs.get(id);
		float angle = 0.0f;
		ds::math::followRelative(target, bomb.position, &angle, 60.0f, 0.02f);
	}
}

// ---------------------------------------
// burst
// ---------------------------------------
void Bombs::burst(ID id, float direction) {
	if (_bombs.contains(id)) {
		Bomb& bomb = _bombs.get(id);
		v2 diff = _context->world_pos - bomb.position;
		v2 n = normalize(diff);
		float angle = ds::vector::calculateRotation(n);
		angle += PI;
		bomb.velocity = ds::vector::getRadialVelocity(angle, 100.0f);
		bomb.state = Bomb::BS_TICKING;
		bomb.color = ds::Color(128, 0, 0, 255);
	}
}

// ---------------------------------------
// get position
// ---------------------------------------
const v2& Bombs::getPosition(ID id) const {
	const Bomb& b = _bombs.get(id);
	return b.position;
}

// ---------------------------------------
// clear
// ---------------------------------------
void Bombs::clear() {
	_bombs.clear();
}

// ---------------------------------------
// render
// ---------------------------------------
void Bombs::render() {
	for (int i = 0; i < _bombs.numObjects; ++i) {
		const Bomb& bomb = _bombs.objects[i];
		ds::sprites::draw(bomb.position, ds::math::buildTexture(40, 360, 40, 40), 0.0f, bomb.scale.x, bomb.scale.y, bomb.color);
		if (bomb.state == Bomb::BS_TICKING) {
			float norm = bomb.timer / _context->settings->gateFlashingTTL;
			drawRing(bomb.position,norm);
		}
	}
}

// ---------------------------------------
// draw ring
// ---------------------------------------
void Bombs::drawRing(const v2& pos,float timer) {
	float step = TWO_PI / 36.0f;
	float angle = TWO_PI * timer * 0.2f;
	ds::Color clr(230, 88, 31, 255);
	clr.r = 0.9 + sin(timer * PI) * 0.1f;
	for (int i = 0; i < 36; ++i) {
		float x = pos.x + cos(angle) * BOMB_EXPLOSION_RADIUS * (1.0f + sin(timer * PI * 3.0f) * 0.1f);
		float y = pos.y + sin(angle) * BOMB_EXPLOSION_RADIUS * (1.0f + sin(timer * PI * 3.0f) * 0.1f);
		float d = _cells[i] + sin(timer * PI * 8.0f) * 0.4f;
		ds::sprites::draw(v2(x,y), ds::math::buildTexture(40, 120, 6, 6), 0.0f, d, d, clr);
		angle += step;
	}
}

// ---------------------------------------
// activate
// ---------------------------------------
void Bombs::activate(int maxCurrent) {
	_spawn_timer = 0.0f;
	_bombs.clear();
	_max_current = maxCurrent;
}

// ---------------------------------------
// scale gates
// ---------------------------------------
void Bombs::scaleBombs(EventBuffer* buffer, float dt) {
	for (int i = 0; i < _bombs.numObjects; ++i) {
		Bomb& bomb = _bombs.objects[i];
		if (bomb.state == Bomb::BS_TICKING) {
			bomb.timer += dt;
			float norm = bomb.timer / _context->settings->gateFlashingTTL;
			if (norm > 1.0f) {
				_context->particles->start(BOMB_EXPLOSION, v3(bomb.position));
				norm = 1.0f;
				buffer->add(GameEvent::GE_BOMB_EXPLODED, bomb.position);
				_bombs.remove(bomb.id);
			}
			else {
				float s = 1.0f + sin(norm * PI * _context->settings->bombFlashAmplitude) * 0.2f;
				bomb.scale.x = s;
				bomb.scale.y = s;
			}
		}
	}
}

// ---------------------------------------
// ticks
// ---------------------------------------
void Bombs::tick(EventBuffer* buffer, float dt) {
	// spawn
	if (_bombs.numObjects < _max_current) {
		_spawn_timer += dt;
		if (_spawn_timer > 1.0f) {
			_spawn_timer = 0.0f;
			create();
		}
	}
	// move
	for (int i = 0; i < _bombs.numObjects; ++i) {
		Bomb& b = _bombs.objects[i];
		if (b.state == Bomb::BS_ACTIVE || b.state == Bomb::BS_TICKING) {
			b.position += b.velocity * dt;
			bool bouncing = false;
			if (b.position.x < 20.0f || b.position.x > 1900.0f) {
				b.velocity.x *= -1.0f;
				bouncing = true;
			}
			if (b.position.y < 20.0f || b.position.y > 1060.0f) {
				b.velocity.y *= -1.0f;
				bouncing = true;
			}
			if (bouncing) {
				b.position += b.velocity * dt;
			}
		}
	}
	
	scaleBombs(buffer, dt);
}

// ---------------------------------------
// check interception
// ---------------------------------------
void Bombs::checkInterception(EventBuffer* buffer, const v2& pos, float radius) {
	for (int i = 0; i < _bombs.numObjects; ++i) {
		Bomb& bomb = _bombs.objects[i];
		if (bomb.state == Bomb::BS_ACTIVE) {
			if (ds::math::checkCircleIntersection(_context->world_pos, PLAYER_RADIUS, bomb.position, 20.0f)) {
				bomb.state = Bomb::BS_TICKING;
				bomb.timer = 0.0f;
				bomb.color = ds::Color(230, 88, 31, 255);
				buffer->add(GameEvent::GE_BOMB_ACTIVATED, bomb.position);
			}
		}
	}
}

// ---------------------------------------
// kill all
// ---------------------------------------
void Bombs::killAll() {
	for (int i = 0; i < _bombs.numObjects; ++i) {
		Bomb& b = _bombs.objects[i];
		_context->particles->start(BOMB_EXPLOSION, v3(b.position));
	}
	_bombs.clear();
}