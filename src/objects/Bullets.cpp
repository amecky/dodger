#include "Bullets.h"
#include <core\math\math.h>
#include "..\Constants.h"
#include <resources\ResourceContainer.h>

// ---------------------------------------
// Bullets
// ---------------------------------------
Bullets::Bullets(ds::World * world, GameSettings* settings) : _world(world) , _settings(settings), _active(false) , _timer(0.0f) {
	_particles = ds::res::getParticleManager();
}

// ---------------------------------------------
// tick
// ---------------------------------------------
void Bullets::tick(float dt) {
	if (_active) {
		{
			ZoneTracker u1("Bullets:emitt");
			_timer += dt;
			if (_timer >= _settings->bullets.rate) {
				_timer -= _settings->bullets.rate;
				v3 wp = _world->getPosition(_player);
				v3 r = _world->getRotation(_player);
				v2 pos = wp.xy();
				math::addRadial(pos, 30.0f, r.x);
				ID bullet = _world->create(pos, math::buildTexture(0, 410, 22, 8), OT_BULLET, r.x, v2(1.0f), ds::Color(249, 246, 194, 255));
				BulletData* data = (BulletData*)_world->attach_data(bullet, sizeof(BulletData), OT_BULLET);
				data->previous = pos;
				data->sqrDist = 10.0f * 10.0f;
				v2 vel = math::getRadialVelocity(r.x, _settings->bullets.velocity);
				_world->setRotation(bullet, r.x);
				_world->moveBy(bullet, vel, -1.0f, false);
				_world->attachCollider(bullet, ds::PST_CIRCLE, v2(18.0f));
			}
		}		
	}
	ZoneTracker u2("Bullets:trail");
	ID ids[64];
	float TTL = 0.1f;
	int num = _world->find_by_type(OT_BULLET, ids, 64);
	for (int i = 0; i < num; ++i) {
		BulletData* data = (BulletData*)_world->get_data(ids[i]);
		const v3& p = _world->getPosition(ids[i]);
		v3 d = p - data->previous;
		if (sqr_length(d) > data->sqrDist) {
			//LOG << "emitting trail";
			//LOG << "id: " << ids[i] << " p: " << DBG_V3(p) << " prev: " << DBG_V3(data->previous);
			const v3& r = _world->getRotation(ids[i]);
			ID trail = _world->create(data->previous.xy(), math::buildTexture(10, 407, 30, 14), OT_BULLET_TRAIL, r.x, v2(1.0f), ds::Color(249, 246, 194, 255));
			_world->alphaFadeTo(trail, 1.0f, 0.0f, TTL);
			_world->scale(trail, v3(1.0f, 0.5f, 0.0f), v3(1.5f, 0.2f, 0.0f), TTL);
			_world->removeAfter(trail, TTL);
			data->previous = p;
		}
	}
}

void Bullets::killAll() {
	ID ids[256];
	int num = _world->find_by_type(OT_BULLET, ids, 256);
	for (int i = 0; i < num; ++i) {
		_world->remove(ids[i]);
	}
	num = _world->find_by_type(OT_BULLET_TRAIL, ids, 256);
	for (int i = 0; i < num; ++i) {
		_world->remove(ids[i]);
	}
}

// ---------------------------------------------
// start
// ---------------------------------------------
void Bullets::start(ID player) {
	if (!_active) {
		_active = true;
		_player = player;
		// ??? start shooting right away
		_timer = 0.0f;
	}
}

// ---------------------------------------------
// stop
// ---------------------------------------------
void Bullets::stop() {
	_active = false;
	_timer = 0.0f;
}

// ---------------------------------------------
// kill
// ---------------------------------------------
void Bullets::kill(ID id) {
	if (_world->contains(id)) {
		v3 pos = _world->getPosition(id);
		_particles->start(8, pos.xy());
		_world->remove(id);
	}
}
