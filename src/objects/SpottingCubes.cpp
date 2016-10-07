#include "SpottingCubes.h"
#include <core\math\math.h>
#include "..\Constants.h"
// ---------------------------------------
// WanderingCubes
// ---------------------------------------
SpottingCubes::SpottingCubes(ds::World* world, GameSettings* settings) : _world(world), _settings(settings), _timer(0.0f), _delay(0.0f), _max(0), _emitted(0) {
	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
}

SpottingCubes::~SpottingCubes() {
}

// ---------------------------------------
// reset
// ---------------------------------------
void SpottingCubes::reset() {
}

// ---------------------------------------
// init
// ---------------------------------------
void SpottingCubes::init(int max, float spawnDelay) {
	_delay = spawnDelay;
	_max = max;
	_timer = 0.0f;
	_emitted = 0;
}

// ---------------------------------------
// tick
// ---------------------------------------
void SpottingCubes::tick(ID player, float dt) {
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
			if (type == OT_SPOTTER) {
				if (event.action == ds::AT_SCALE_BY_PATH) {					
					_world->rotateTo(event.id, player, PI);
				}
				else if (event.action == ds::AT_ROTATE_TO_TARGET) {
					v3 rot = _world->getRotation(event.id);
					v3 v = math::getRadialVelocity(rot.x, 150.0f);
					float ttl = math::random(4.0f, 8.0f);
					_world->moveByFinite(event.id, v, ttl);
				}
				else if (event.action == ds::AT_MOVE_BY_FINITE) {
					_world->rotateTo(event.id, player, 1.0f);
				}
			}
		}
	}
}

// ---------------------------------------
// create
// ---------------------------------------
void SpottingCubes::create() {
	float x = 512.0f;// math::random(200.0f, 800.0f);
	float y = 384.0f;// math::random(200.0f, 550.0f);
	ID id = _world->create(v2(x, y), math::buildTexture(40, 320, 60, 60), OT_SPOTTER);
	_world->scaleByPath(id, &_scale_path, 0.8f);
}

