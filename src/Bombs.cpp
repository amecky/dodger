#include "Bombs.h"


Bombs::Bombs(GameContext* context) : _context(context) {
	float step = TWO_PI / 36.0f;
	for (int i = 0; i < 36; ++i) {
		_cells[i] = 0.9f + sin(step * static_cast<float>(i) * 12.0f) * 0.4f;
	}
	_texture = ds::math::buildTexture(0, 440, 60, 60);
	_ring_texture = ds::math::buildTexture(40, 120, 6, 6);
	_scale_path.add(0.0f, v2(0.1f,0.1f));
	_scale_path.add(0.5f, v2(1.5f,1.5f));
	_scale_path.add(0.75f, v2(0.75f,0.75f));
	_scale_path.add(1.0f, v2(1.0f,1.0f));
}


Bombs::~Bombs() {
}

// ---------------------------------------
// create
// ---------------------------------------
void Bombs::create() {
	ID id = _bombs.add();
	Bomb& b = _bombs.get(id);
	b.position.x = ds::math::random(200.0f, 1400.0f);
	b.position.y = ds::math::random(200.0f, 700.0f);
	float angle = ds::math::random(0.0f, TWO_PI);
	float v = ds::math::random(30.0f, 50.0f);
	b.velocity = ds::vector::getRadialVelocity(angle, v);
	b.scale = v2(1, 1);
	b.state = Bomb::BS_STARTING;
	b.resetTimer();
	b.color = ds::Color(130, 0, 255, 255);
	b.rotation = 0.0f;
	b.texture = _texture;
	_context->particles->start(BOMB_STARTUP, v3(b.position));
}

// ---------------------------------------
// grab bomb
// ---------------------------------------
bool Bombs::grab(const v2& pos, float radius, ID* id) {
	BombArray::iterator it = _bombs.begin();
	while (it != _bombs.end()) {
		if (it->state == Bomb::BS_ACTIVE) {
			if (ds::math::checkCircleIntersection(_context->world_pos, radius, it->position, 20.0f)) {
				it->state = Bomb::BS_FOLLOWING;
				it->color = ds::Color(0, 192, 32, 255);
				*id = it->id;
				return true;
			}
		}
		++it;
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
		v2 diff = _context->world_pos - bomb.position;
		v2 n = normalize(diff);
		bomb.rotation = ds::vector::calculateRotation(n);
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
	BombArray::iterator it = _bombs.begin();
	while (it != _bombs.end()) {
		ds::sprites::draw(*it);
		if (it->state == Bomb::BS_TICKING) {
			float norm = it->timer / _context->settings->bombFlashingTTL;
			drawRing(it->position,norm);
		}
		++it;
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
		ds::sprites::draw(v2(x,y), _ring_texture, 0.0f, d, d, clr);
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
	BombArray::iterator it = _bombs.begin();
	while (it != _bombs.end()) {
		if (it->state == Bomb::BS_TICKING) {
			if (it->tickTimer(dt, _context->settings->bombFlashingTTL)) {
				_context->particles->start(BOMB_EXPLOSION, v3(it->position));
				buffer->add(GameEvent::GE_BOMB_EXPLODED, it->position);
				it = _bombs.remove(it->id);
			}
			else {
				_scale_path.get(it->normalizedTimer, &it->scale);
				++it;
			}
		}
		else {
			++it;
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
	BombArray::iterator it = _bombs.begin();
	while (it != _bombs.end()) {
		if (it->state == Bomb::BS_ACTIVE || it->state == Bomb::BS_TICKING) {
			it->position += it->velocity * dt;
			bool bouncing = false;
			if (it->position.x < 130.0f || it->position.x > 1470.0f) {
				it->velocity.x *= -1.0f;
				bouncing = true;
			}
			if (it->position.y < 130.0f || it->position.y > 770.0f) {
				it->velocity.y *= -1.0f;
				bouncing = true;
			}
			if (bouncing) {
				it->position += it->velocity * dt;
			}
			v2 n = normalize(it->velocity);
			it->rotation = ds::vector::calculateRotation(n);
		}
		if (it->state == Bomb::BS_STARTING) {
			if (it->tickTimer(dt, _context->settings->bombStartTTL)) {
				it->state = Bomb::BS_ACTIVE;
			}
			_scale_path.get(it->normalizedTimer, &it->scale);
			v2 n = normalize(it->velocity);
			it->rotation = ds::vector::calculateRotation(n);
		}
		++it;
	}	
	scaleBombs(buffer, dt);
}

// ---------------------------------------
// check interception
// ---------------------------------------
void Bombs::checkInterception(EventBuffer* buffer, const v2& pos, float radius) {
	BombArray::iterator it = _bombs.begin();
	while (it != _bombs.end()) {
		if (it->state == Bomb::BS_ACTIVE) {
			if (ds::math::checkCircleIntersection(_context->world_pos, PLAYER_RADIUS, it->position, 20.0f)) {
				it->state = Bomb::BS_TICKING;
				it->resetTimer();
				it->color = ds::Color(230, 88, 31, 255);
				buffer->add(GameEvent::GE_BOMB_ACTIVATED, it->position);
			}
		}
		++it;
	}
}

// ---------------------------------------
// kill all
// ---------------------------------------
void Bombs::killAll() {
	BombArray::iterator it = _bombs.begin();
	while (it != _bombs.end()) {
		_context->particles->start(BOMB_EXPLOSION, v3(it->position));
		++it;
	}
	_bombs.clear();
}