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

// ---------------------------------------------
// create
// ---------------------------------------------
void Bullets::create(float offset) {
	v3 wp = _world->getPosition(_player);
	v3 r = _world->getRotation(_player);
	v2 p = wp.xy();
	float oa = r.x + offset;
	math::addRadial(p, 30.0f, oa);
	ID bullet = _world->create(p, SID("Bullet"));// math::buildTexture(0, 410, 22, 8), OT_BULLET, r.x, v2(1.0f), ds::Color(249, 246, 194, 255));
	_world->setRotation(bullet, r.x);
	_world->startBehavior(SID("bullets"), bullet);
	_world->attachCollider(bullet, ds::PST_CIRCLE, v2(18.0f));
}

// ---------------------------------------------
// tick
// ---------------------------------------------
void Bullets::tick(float dt) {
	if (_active) {
		ZoneTracker u1("Bullets:emitt");
		_timer += dt;
		if (_timer >= _settings->bullets.rate) {
			_timer -= _settings->bullets.rate;
			create(DEGTORAD(15.0f));
			create(-DEGTORAD(15.0f));
		}		
	}
}

// ---------------------------------------------
// kill all
// ---------------------------------------------
void Bullets::killAll() {
	_world->removeByType(OT_BULLET);
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
