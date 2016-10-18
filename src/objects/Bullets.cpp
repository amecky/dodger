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
		_timer += dt;
		if (_timer >= _settings->bullets.rate) {
			_timer -= _settings->bullets.rate;
			v3 wp = _world->getPosition(_player);
			v3 r = _world->getRotation(_player);
			v2 pos = wp.xy();
			math::addRadial(pos, 30.0f, r.x);
			ID bullet = _world->create(pos, math::buildTexture(0, 410, 22, 8), OT_BULLET, r.x, v2(1.0f), ds::Color(249, 246, 194, 255));
			v2 vel = math::getRadialVelocity(r.x, _settings->bullets.velocity);
			_world->setRotation(bullet, r.x);
			_world->moveBy(bullet, vel, -1.0f, false);
			_world->attachCollider(bullet, ds::PST_CIRCLE, v2(18.0f));
		}
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
