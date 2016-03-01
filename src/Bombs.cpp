#include "Bombs.h"
#include <utils\Log.h>

Bombs::Bombs(GameContext* context) : _context(context) , _world(context->world) {
	float step = TWO_PI / 36.0f;
	for (int i = 0; i < 36; ++i) {
		_cells[i] = 0.9f + sin(step * static_cast<float>(i) * 12.0f) * 0.4f;
	}
	//_texture = ds::math::buildTexture(0, 440, 60, 60);
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
	BombData* data = (BombData*)_world->attach_data(sid, sizeof(BombData));
	float angle = ds::math::random(0.0f, TWO_PI);
	float v = ds::math::random(30.0f, 50.0f);
	v2 vel = ds::vector::getRadialVelocity(angle, v);
	_world->moveBy(sid, vel, true);
	_world->setRotation(sid, vel);
	_world->scaleByPath(sid, &_scale_path, _context->settings->bombFlashingTTL);
	data->state = BombData::BS_STARTING;
	_context->particles->start(BOMB_STARTUP, pos);
}

void Bombs::handleEvents(const ds::ActionEventBuffer& buffer) {
	for (int i = 0; i < buffer.events.size(); ++i) {
		const ds::ActionEvent& event = buffer.events[i];
		if (_world->contains(event.sid)) {
			int type = _world->getType(event.sid);
			if (type == OT_BOMB) {
				if (event.type == ds::AT_SCALE_BY_PATH) {
					BombData* data = (BombData*)_world->get_data(event.sid);
					assert(data != 0);
					data->state = BombData::BS_ACTIVE;
				}
			}
		}
	}
}
// ---------------------------------------
// grab bomb
// ---------------------------------------
bool Bombs::grab(const v2& pos, float radius, ds::SID* id) {
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	for (int i = 0; i < num; ++i) {
		BombData* data = (BombData*)_world->get_data(_bomb_sids[i]);
		assert(data != 0);
		if (data->state == BombData::BS_ACTIVE) {
			v2 p = _world->getPosition(_bomb_sids[i]);
			if (ds::math::checkCircleIntersection(_context->world_pos, radius, p, 20.0f)) {
				data->state = BombData::BS_FOLLOWING;
				_world->stopAction(_bomb_sids[i], ds::AT_MOVE_BY);
				_world->setColor(_bomb_sids[i], ds::Color(0, 192, 32, 255));
				*id = _bomb_sids[i];
				return true;
			}
		}
	}
	return false;
}

// ---------------------------------------
// follow target
// ---------------------------------------
void Bombs::follow(ds::SID id, const v2& target) {
	if (_world->contains(id)) {
		v2 p = _world->getPosition(id);
		float angle = 0.0f;
		ds::math::followRelative(target,p, &angle, 60.0f, 0.02f);
		v2 diff = _context->world_pos - p;
		v2 n = normalize(diff);
		_world->setPosition(id, p);
		_world->setRotation(id,  ds::vector::calculateRotation(n));
	}
}

// ---------------------------------------
// burst
// ---------------------------------------
void Bombs::burst(ds::SID id, float direction) {
	if (_world->contains(id)) {
		BombData* data = (BombData*)_world->get_data(id);
		assert(data != 0);
		v2 p = _world->getPosition(id);
		v2 diff = _context->world_pos - p;
		v2 n = normalize(diff);
		float angle = ds::vector::calculateRotation(n);
		angle += PI;
		v2 vel = ds::vector::getRadialVelocity(angle, 100.0f);
		_world->moveBy(id,vel);
		data->state = BombData::BS_TICKING;
		data->timer = 0.0f;
		//_world->setColor(bomb.sid, ds::Color(128, 0, 0, 255));
		_world->flashColor(id, ds::Color(138, 39, 0), ds::Color(255, 165, 130), 0.3f, -1);
		_world->setRotation(id, vel);
	}
}

// ---------------------------------------
// clear
// ---------------------------------------
void Bombs::clear() {
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	for (int i = 0; i < num; ++i) {
		_world->remove(_bomb_sids[i]);
	}
}

// ---------------------------------------
// render
// ---------------------------------------
void Bombs::render() {
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	for (int i = 0; i < num; ++i) {
		BombData* data = (BombData*)_world->get_data(_bomb_sids[i]);
		assert(data != 0);
		if (data->state == BombData::BS_TICKING) {
			float norm = data->timer / _context->settings->bombFlashingTTL;
			v2 p = _world->getPosition(_bomb_sids[i]);
			drawRing(p ,norm);
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
		ds::sprites::draw(v2(x,y), _ring_texture, 0.0f, d, d, clr);
		angle += step;
	}
}

// ---------------------------------------
// activate
// ---------------------------------------
void Bombs::activate() {
	_spawn_timer = 0.0f;
	clear();
}

// ---------------------------------------
// scale gates
// ---------------------------------------
void Bombs::scaleBombs(EventBuffer* buffer, float dt) {
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	for (int i = 0; i < num; ++i) {
		BombData* data = (BombData*)_world->get_data(_bomb_sids[i]);
		assert(data != 0);
		if (data->state == BombData::BS_TICKING) {
			data->timer += dt;
			if (data->timer >= _context->settings->bombFlashingTTL) {
				v2 p = _world->getPosition(_bomb_sids[i]);
				_context->particles->startGroup(1, p);
				buffer->add(GameEvent::GE_BOMB_EXPLODED, p);
				_world->remove(_bomb_sids[i]);
			}
		}
	}
}

// ---------------------------------------
// ticks
// ---------------------------------------
void Bombs::tick(EventBuffer* buffer, float dt) {
	// spawn
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	if (num < _context->settings->maxBombs) {
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
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	for (int i = 0; i < num; ++i) {
		BombData* data = (BombData*)_world->get_data(_bomb_sids[i]);
		assert(data != 0);
		if (data->state == BombData::BS_ACTIVE) {
			v2 p = _world->getPosition(_bomb_sids[i]);
			if (ds::math::checkCircleIntersection(_context->world_pos, PLAYER_RADIUS, p, 20.0f)) {
				data->state = BombData::BS_TICKING;
				data->timer = 0.0f;
				_world->flashColor(_bomb_sids[i], ds::Color(138, 39, 0), ds::Color(255, 135, 88), 0.4f, -1);
				buffer->add(GameEvent::GE_BOMB_ACTIVATED, p);
			}
		}
	}
}

// ---------------------------------------
// kill all
// ---------------------------------------
void Bombs::killAll() {
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	for (int i = 0; i < num; ++i) {
		v2 p = _world->getPosition(_bomb_sids[i]);
		_context->particles->start(BOMB_EXPLOSION, p);
		_world->remove(_bomb_sids[i]);
	}
}