#include "FollowerCubes.h"
#include <core\math\math.h>
#include "..\Constants.h"
// ---------------------------------------
// WanderingCubes
// ---------------------------------------
FollowerCubes::FollowerCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings) : BasicCubes(world, emitter, settings) {
}

FollowerCubes::~FollowerCubes() {
}

// ---------------------------------------
// handle events
// ---------------------------------------
void FollowerCubes::onEvent(const ds::ActionEvent& event, ID target, float dt) {
	if (event.action == ds::AT_SCALE_BY_PATH) {			
		_world->attachCollider(event.id, ds::PST_CIRCLE, v2(42.0f, 42.0f));
		_world->seek(event.id, target, 150.0f);
		_world->separate(event.id, OT_FOLLOWER, 30.0f, 0.9f);
	}
}

// ---------------------------------------
// create
// ---------------------------------------
void FollowerCubes::create(ID target, int num) {
	createCubes(SID("Follower"), target, num, 1);	
}

