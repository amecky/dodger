#include "Bombs.h"


Bombs::Bombs(GameContext* context) : _context(context) , _world(context->world) {
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
	v2 pos;
	pos.x = ds::math::random(200.0f, 1400.0f);
	pos.y = ds::math::random(200.0f, 700.0f);
	ds::SID sid = _world->create(pos, "Bomb");
	ID id = _bombs.add();
	Bomb& b = _bombs.get(id);
	b.sid = sid;
	float angle = ds::math::random(0.0f, TWO_PI);
	float v = ds::math::random(30.0f, 50.0f);
	v2 vel = ds::vector::getRadialVelocity(angle, v);
	_world->moveBy(sid, vel, true);
	_world->setRotation(sid, vel);
	_world->scaleByPath(sid, &_scale_path, _context->settings->bombFlashingTTL);
	b.state = Bomb::BS_STARTING;
	b.resetTimer();
	_context->particles->start(BOMB_STARTUP, v3(pos));
}

void Bombs::handleEvents(const ds::ActionEventBuffer& buffer) {
	for (int i = 0; i < buffer.events.size(); ++i) {
		const ds::ActionEvent& event = buffer.events[i];
		if (event.type == ds::AT_SCALE_BY_PATH) {
			BombArray::iterator it = _bombs.begin();
			while (it != _bombs.end()) {
				if (it->sid == event.sid) {
					it->state = Bomb::BS_ACTIVE;
				}
				++it;
			}
		}
	}
}
// ---------------------------------------
// grab bomb
// ---------------------------------------
bool Bombs::grab(const v2& pos, float radius, ID* id) {
	BombArray::iterator it = _bombs.begin();
	while (it != _bombs.end()) {
		if (it->state == Bomb::BS_ACTIVE) {
			v2 p = _world->getPosition(it->sid);
			if (ds::math::checkCircleIntersection(_context->world_pos, radius, p, 20.0f)) {
				it->state = Bomb::BS_FOLLOWING;
				_world->stopAction(it->sid, ds::AT_MOVE_BY);
				_world->setColor(it->sid, ds::Color(0, 192, 32, 255));
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
		v2 p = _world->getPosition(bomb.sid);
		float angle = 0.0f;
		ds::math::followRelative(target,p, &angle, 60.0f, 0.02f);
		v2 diff = _context->world_pos - p;
		v2 n = normalize(diff);
		_world->setPosition(bomb.sid, p);
		_world->setRotation(bomb.sid,  ds::vector::calculateRotation(n));
	}
}

// ---------------------------------------
// burst
// ---------------------------------------
void Bombs::burst(ID id, float direction) {
	if (_bombs.contains(id)) {
		Bomb& bomb = _bombs.get(id);
		v2 p = _world->getPosition(bomb.sid);
		v2 diff = _context->world_pos - p;
		v2 n = normalize(diff);
		float angle = ds::vector::calculateRotation(n);
		angle += PI;
		v2 vel = ds::vector::getRadialVelocity(angle, 100.0f);
		_world->moveBy(bomb.sid,vel);
		bomb.state = Bomb::BS_TICKING;
		_world->setColor(bomb.sid, ds::Color(128, 0, 0, 255));
		_world->setRotation(bomb.sid, vel);
	}
}

// ---------------------------------------
// get position
// ---------------------------------------
const v2& Bombs::getPosition(ID id) const {
	const Bomb& b = _bombs.get(id);
	return _world->getPosition(b.sid);
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
		if (it->state == Bomb::BS_TICKING) {
			float norm = it->timer / _context->settings->bombFlashingTTL;
			v2 p = _world->getPosition(it->sid);
			drawRing(p ,norm);
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
void Bombs::activate() {
	_spawn_timer = 0.0f;
	_bombs.clear();
}

// ---------------------------------------
// scale gates
// ---------------------------------------
void Bombs::scaleBombs(EventBuffer* buffer, float dt) {
	BombArray::iterator it = _bombs.begin();
	while (it != _bombs.end()) {
		if (it->state == Bomb::BS_TICKING) {
			if (it->tickTimer(dt, _context->settings->bombFlashingTTL)) {
				v2 p = _world->getPosition(it->sid);
				_context->particles->start(BOMB_EXPLOSION, v3(p));
				_context->particles->start(BOMB_RING_EXPLOSION, v3(p));
				_context->particles->start(BOMB_DEBRIS, v3(p));
				buffer->add(GameEvent::GE_BOMB_EXPLODED, p);
				_world->remove(it->sid);
				it = _bombs.remove(it->id);
			}
			else {
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
	if (_bombs.numObjects < _context->settings->maxBombs) {
		_spawn_timer += dt;
		if (_spawn_timer > _context->settings->bombStartTTL) {
			_spawn_timer = 0.0f;
			create();
		}
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
			v2 p = _world->getPosition(it->sid);
			if (ds::math::checkCircleIntersection(_context->world_pos, PLAYER_RADIUS, p, 20.0f)) {
				it->state = Bomb::BS_TICKING;
				it->resetTimer();
				_world->setColor(it->sid, ds::Color(230, 88, 31, 255));
				buffer->add(GameEvent::GE_BOMB_ACTIVATED, p);
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
		v2 p = _world->getPosition(it->sid);
		_context->particles->start(BOMB_EXPLOSION, p);
		_world->remove(it->sid);
		++it;
	}
	_bombs.clear();
}