#include "SpottingCubes.h"
#include <core\math\math.h>
#include "..\Constants.h"
// ---------------------------------------
// WanderingCubes
// ---------------------------------------
SpottingCubes::SpottingCubes(ds::World* world, GameSettings* settings) : BasicCubes(world,settings) {
	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
}

SpottingCubes::~SpottingCubes() {
}

// ---------------------------------------
// handle events
// ---------------------------------------
void SpottingCubes::handleEvents(ID target, float dt) {
	if (_world->hasEvents()) {
		uint32_t n = _world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _world->getEvent(i);
			int type = _world->getType(event.id);
			if (type == OT_SPOTTER) {
				if (event.action == ds::AT_SCALE_BY_PATH) {					
					_world->lookAt(event.id, target, 1.0f);
				}
				else if (event.action == ds::AT_LOOK_AT) {
					v3 rot = _world->getRotation(event.id);
					v3 v = math::getRadialVelocity(rot.x, 150.0f);
					float ttl = math::random(4.0f, 8.0f);
					_world->moveBy(event.id, v, ttl);
				}
				else if (event.action == ds::AT_MOVE_BY) {
					_world->lookAt(event.id, target, 1.0f);
				}
			}
		}
	}
}

// ---------------------------------------
// create
// ---------------------------------------
void SpottingCubes::create(const CubeEmitter& emitter) {
	ID id = _world->create(emitter.next(), math::buildTexture(130, 410, 52, 52), OT_SPOTTER);
	_world->scaleByPath(id, &_scale_path, 0.8f);

}

