#include "WanderingCubes.h"
#include <core\math\math.h>

// ---------------------------------------
// WanderingCubes
// ---------------------------------------
WanderingCubes::WanderingCubes(ds::World* world,GameSettings* settings) : _world(world), _settings(settings), _timer(0.0f), _delay(0.0f), _max(0), _emitted(0) {
	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
}

WanderingCubes::~WanderingCubes() {
}

// ---------------------------------------
// reset
// ---------------------------------------
void WanderingCubes::reset() {
}

// ---------------------------------------
// init
// ---------------------------------------
void WanderingCubes::init(int max, float spawnDelay) {
	_delay = spawnDelay;
	_max = max;
	_timer = 0.0f;
	_emitted = 0;
}

// ---------------------------------------
// tick
// ---------------------------------------
void WanderingCubes::tick(float dt) {
	if (_emitted < _max) {
		_timer += dt;
		if (_timer > _delay) {
			// emitt ball
			//create();
			//++_emitted;
		}
	}

	if (_world->hasEvents()) {
		uint32_t n = _world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _world->getEvent(i);
			int type = _world->getType(event.id);
			if ( type == 30 ) {
				if (event.action == ds::AT_SCALE_BY_PATH) {
					float angle = math::random(_settings->wanderer.minRotationAngle, _settings->wanderer.maxRotationAngle);
					_world->rotateBy(event.id, DEGTORAD(angle), 1.0f);
				}
				else if (event.action == ds::AT_ROTATE_BY) {
					v3 rot = _world->getRotation(event.id);
					v3 v = math::getRadialVelocity(rot.x, 50.0f);
					float ttl = math::random(4.0f, 8.0f);
					_world->moveByFinite(event.id, v, ttl);
				}
				else if (event.action == ds::AT_MOVE_BY_FINITE) {
					float angle = math::random(_settings->wanderer.minRotationAngle, _settings->wanderer.maxRotationAngle);
					_world->rotateBy(event.id, DEGTORAD(angle), 1.0f);
				}
			}
		}
	}
}

// ---------------------------------------
// create
// ---------------------------------------
void WanderingCubes::create() {
	float x = math::random(200.0f, 800.0f);
	float y = math::random(200.0f, 550.0f);
	ID id = _world->create(v2(x, y), math::buildTexture(40, 280, 40, 40), 30);
	_world->scaleByPath(id, &_scale_path, 0.8f);
}

