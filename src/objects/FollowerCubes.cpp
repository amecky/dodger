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
	_emitter->next();
	for (int i = 0; i < num; ++i) {
		v2 p = _emitter->get(i, num);
		ID id = _world->create(_emitter->get(i, 13), SID("Follower")); 
		float ttl = math::random(0.5f, 0.8f);
		_world->scaleByPath(id, &_scale_path, ttl);		
		rotateTo(id, target);
		CubeData* data = (CubeData*)_world->attach_data(id, sizeof(CubeData), OT_FOLLOWER);
		data->energy = 1;
	}

}

