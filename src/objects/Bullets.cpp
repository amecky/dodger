#include "Bullets.h"
#include <core\math\math.h>
#include "..\Constants.h"
#include <resources\ResourceContainer.h>
#include <particles\modules\PointEmitterModule.h>
// ---------------------------------------
// Bullets
// ---------------------------------------
Bullets::Bullets(ds::World * world, GameSettings* settings) : _world(world) , _settings(settings), _active(false) , _timer(0.0f) {
	_particles = ds::res::getParticleManager();
}

void Bullets::create(float offset) {
	v3 wp = _world->getPosition(_player);
	v3 r = _world->getRotation(_player);
	v2 p = wp.xy();
	float oa = r.x + offset;
	math::addRadial(p, 30.0f, oa);
	ID bullet = _world->create(p, math::buildTexture(0, 410, 22, 8), OT_BULLET, r.x, v2(1.0f), ds::Color(249, 246, 194, 255));
	//BulletData* data = (BulletData*)_world->attach_data(bullet, sizeof(BulletData), OT_BULLET);
	//data->previous = pos;
	//data->sqrDist = 4.0f * 4.0f;
	//v2 vel = math::getRadialVelocity(math::randomRange(r.x, DEGTORAD(5.0f)), _settings->bullets.velocity);
	v2 vel = math::getRadialVelocity(r.x, _settings->bullets.velocity);
	_world->setRotation(bullet, r.x);
	_world->moveBy(bullet, vel, -1.0f, false);
	_world->attachCollider(bullet, ds::PST_CIRCLE, v2(18.0f));
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
				create(DEGTORAD(15.0f));
				create(-DEGTORAD(15.0f));
				/*
				v3 wp = _world->getPosition(_player);
				v3 r = _world->getRotation(_player);
				v2 pos = wp.xy();
				math::addRadial(pos, 30.0f, r.x);
				ID bullet = _world->create(pos, math::buildTexture(0, 410, 22, 8), OT_BULLET, r.x, v2(1.0f), ds::Color(249, 246, 194, 255));
				BulletData* data = (BulletData*)_world->attach_data(bullet, sizeof(BulletData), OT_BULLET);
				data->previous = pos;
				data->sqrDist = 4.0f * 4.0f;
				v2 vel = math::getRadialVelocity(math::randomRange(r.x, DEGTORAD(5.0f)), _settings->bullets.velocity);
				_world->setRotation(bullet, r.x);
				_world->moveBy(bullet, vel, -1.0f, false);
				_world->attachCollider(bullet, ds::PST_CIRCLE, v2(18.0f));
				*/
			}
		}		
	}
	/*
	ZoneTracker u2("Bullets:trail");
	ID ids[64];
	float TTL = 0.1f;
	int num = _world->find_by_type(OT_BULLET, ids, 64);
	for (int i = 0; i < num; ++i) {
		BulletData* data = (BulletData*)_world->get_data(ids[i]);
		const v3& p = _world->getPosition(ids[i]);
		v3 d = p - data->previous;
		if (sqr_length(d) > data->sqrDist) {
			const v3& r = _world->getRotation(ids[i]);
			ds::ParticleSystem* system = _particles->getParticleSystem(11);
			ds::PointEmitterModuleData* peData = (ds::PointEmitterModuleData*)system->getData(ds::PM_POINT);
			peData->rotation = r.x;
			_particles->start(11, data->previous.xy());
			data->previous = p;
		}
	}
	*/
}

// ---------------------------------------------
// kill all
// ---------------------------------------------
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
	_active = false;
	_player = INVALID_ID;
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
