#include "FollowerCubes.h"
#include <core\math\math.h>
#include "..\Constants.h"
// ---------------------------------------
// WanderingCubes
// ---------------------------------------
FollowerCubes::FollowerCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings) : BasicCubes(world, emitter, settings) {
	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
}

FollowerCubes::~FollowerCubes() {
}

// ---------------------------------------
// handle events
// ---------------------------------------
void FollowerCubes::onEvent(const ds::ActionEvent& event, ID target, float dt) {
	if (event.action == ds::AT_SCALE_BY_PATH) {					
		_world->seek(event.id, target, 150.0f);
		_world->separate(event.id, OT_FOLLOWER, 30.0f, 0.9f);
	}
	/*
	else if (event.action == ds::AT_LOOK_AT) {
		v3 rot = _world->getRotation(event.id);
		v3 v = math::getRadialVelocity(rot.x, 150.0f);
		float ttl = math::random(4.0f, 8.0f);
		_world->moveBy(event.id, v, ttl);
	}
	else if (event.action == ds::AT_MOVE_BY) {
		_world->lookAt(event.id, target, 1.0f);
	}
	*/
}

// ---------------------------------------
// create
// ---------------------------------------
void FollowerCubes::create() {
	_emitter->next();
	for (int i = 0; i < 13; ++i) {
		ID id = _world->create(_emitter->get(i,13), math::buildTexture(190, 410, 42, 42), OT_FOLLOWER);
		float ttl = math::random(0.5f, 0.8f);
		_world->scaleByPath(id, &_scale_path, ttl);
	}

}

