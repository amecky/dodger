#include "Bombs.h"
#include <core\log\Log.h>
#include <core\math\GameMath.h>

Bombs::Bombs(GameContext* context, ds::World* world) : _context(context), _world(world) {
	float step = TWO_PI / 36.0f;
	for (int i = 0; i < 36; ++i) {
		_cells[i] = 0.9f + sin(step * static_cast<float>(i) * 12.0f) * 0.4f;
	}
	//_texture = ds::math::buildTexture(0, 440, 60, 60);
	_ring_texture = math::buildTexture(40, 120, 6, 6);
	_scale_path.add(0.0f, v3(0.1f,0.1f,0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
}


Bombs::~Bombs() {
}

// ---------------------------------------
// create
// ---------------------------------------
void Bombs::create() {
	v2 pos;
	pos.x = math::random(200.0f, 800.0f);
	pos.y = math::random(200.0f, 600.0f);
	ID sid = _world->create(pos, math::buildTexture(0, 440, 60, 60),OT_BOMB);
	_world->attachCollider(sid, ds::PST_CIRCLE, v2(60.0f, 0.0f));
	BombData* data = (BombData*)_world->attach_data(sid, sizeof(BombData),OT_BOMB);
	float angle = math::random(0.0f, TWO_PI);
	float v = math::random(30.0f, 50.0f);
	v2 vel = math::getRadialVelocity(angle, v);
	_world->moveBy(sid, vel, true);
	_world->scaleByPath(sid, &_scale_path, _context->settings->bombFlashingTTL);
	data->state = BombData::BS_STARTING;
	data->velocity = vel;
	//_context->particles->start(BOMB_STARTUP, pos);
}

void Bombs::handleEvents(const ds::ActionEventBuffer& buffer) {
	/*
	for (int i = 0; i < buffer.events.size(); ++i) {
		const ds::ActionEvent& event = buffer.events[i];
		if (_world->contains(event.id)) {
			int type = _world->getType(event.id);
			if (type == OT_BOMB) {
				if (event.type == ds::AT_SCALE_BY_PATH) {
					BombData* data = (BombData*)_world->get_data(event.id);
					assert(data != 0);
					data->state = BombData::BS_ACTIVE;
				}
			}
		}
	}
	*/
}
// ---------------------------------------
// grab bomb
// ---------------------------------------
bool Bombs::grab(const v2& pos, float radius, ID* id) {
	ID ids[64];
	int num = _world->find_by_type(OT_BOMB, ids, 64);
	for (int i = 0; i < num; ++i) {
		BombData* data = (BombData*)_world->get_data(ids[i]);
		assert(data != 0);
		//if (data->state == BombData::BS_ACTIVE) {
			v3 p = _world->getPosition(ids[i]);
			if (ds::physics::testCircleIntersection(pos, radius, p.xy(), 20.0f)) {
				data->state = BombData::BS_FOLLOWING;
				_world->stopAction(ids[i], ds::AT_MOVE_BY);
				_world->setColor(ids[i], ds::Color(0, 192, 32, 255));
				*id = ids[i];
				return true;
			}
		//}
	}
	return false;
}

// ---------------------------------------
// follow target
// ---------------------------------------
void Bombs::follow(ID id, const v2& target, float dt) {
	if (_world->contains(id)) {
		v3 p = _world->getPosition(id);
		float angle = 0.0f;
		v2 d = p.xy();
		ds::math::followRelative(target,d, &angle, 60.0f, dt);
		v3 diff = v3(target) - p;
		v3 n = normalize(diff);
		_world->setPosition(id, d);
		_world->setRotation(id,  math::calculateRotation(n.xy()));
	}
}

// ---------------------------------------
// burst
// ---------------------------------------
void Bombs::burst(ID id, float direction) {
	/*
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
	*/
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
	/*
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	for (int i = 0; i < num; ++i) {
		BombData* data = (BombData*)_world->get_data(_bomb_sids[i]);
		assert(data != 0);
		if (data->state == BombData::BS_TICKING) {
			float norm = data->timer / _context->settings->bombFlashingTTL;
			v3 p = _world->getPosition(_bomb_sids[i]);
			drawRing(p.xy() ,norm);
		}
	}
	*/
}

// ---------------------------------------
// draw ring
// ---------------------------------------
void Bombs::drawRing(const v2& pos,float timer) {
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	float step = TWO_PI / 36.0f;
	float angle = TWO_PI * timer * 0.2f;
	ds::Color clr(230, 88, 31, 255);
	clr.r = 0.9 + sin(timer * PI) * 0.1f;
	for (int i = 0; i < 36; ++i) {
		float x = pos.x + cos(angle) * BOMB_EXPLOSION_RADIUS * (1.0f + sin(timer * PI * 3.0f) * 0.1f);
		float y = pos.y + sin(angle) * BOMB_EXPLOSION_RADIUS * (1.0f + sin(timer * PI * 3.0f) * 0.1f);
		float d = _cells[i] + sin(timer * PI * 8.0f) * 0.4f;
		sprites->draw(v2(x,y), _ring_texture, 0.0f, v2(d, d), clr);
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
	/*
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	for (int i = 0; i < num; ++i) {
		BombData* data = (BombData*)_world->get_data(_bomb_sids[i]);
		assert(data != 0);
		if (data->state == BombData::BS_TICKING) {
			data->timer += dt;
			if (data->timer >= _context->settings->bombFlashingTTL) {
				v3 p = _world->getPosition(_bomb_sids[i]);
				_context->particles->startGroup(1, p);
				buffer->add(GameEvent::GE_BOMB_EXPLODED, p);
				_world->remove(_bomb_sids[i]);
			}
		}
	}
	*/
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
	/*
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	for (int i = 0; i < num; ++i) {
		BombData* data = (BombData*)_world->get_data(_bomb_sids[i]);
		assert(data != 0);
		if (data->state == BombData::BS_ACTIVE) {
			v3 p = _world->getPosition(_bomb_sids[i]);
			if (ds::math::checkCircleIntersection(_context->world_pos, PLAYER_RADIUS, p, 20.0f)) {
				data->state = BombData::BS_TICKING;
				data->timer = 0.0f;
				_world->flashColor(_bomb_sids[i], ds::Color(138, 39, 0), ds::Color(255, 135, 88), 0.4f, -1);
				buffer->add(GameEvent::GE_BOMB_ACTIVATED, p);
			}
		}
	}
	*/
}

// ---------------------------------------
// kill all
// ---------------------------------------
void Bombs::killAll() {
	int num = _world->find_by_type(OT_BOMB, _bomb_sids, 16);
	for (int i = 0; i < num; ++i) {
		v3 p = _world->getPosition(_bomb_sids[i]);
		//_context->particles->start(BOMB_EXPLOSION, p);
		_world->remove(_bomb_sids[i]);
	}
}