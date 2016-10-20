#include "SpottingCubes.h"
#include <core\math\math.h>
#include "..\Constants.h"
// ---------------------------------------
// WanderingCubes
// ---------------------------------------
SpottingCubes::SpottingCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings) : BasicCubes(world, emitter, settings) {
}

SpottingCubes::~SpottingCubes() {
}

// ---------------------------------------
// handle events
// ---------------------------------------
void SpottingCubes::onEvent(const ds::ActionEvent& event, ID target, float dt) {
	if (event.action == ds::AT_SCALE_BY_PATH) {					
		_world->lookAt(event.id, target, 1.0f);
		_world->attachCollider(event.id, ds::PST_CIRCLE, v2(48.0f, 48.0f));
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

// ---------------------------------------
// create
// ---------------------------------------
void SpottingCubes::create(ID target, int num) {
	createCubes(SID("Spotter"), target, num, 4);	
}

