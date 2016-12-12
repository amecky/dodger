#include "EnemyBehavior.h"
/*
void FollowerBehavior::create(ID id, ID target) {
	float ttl = math::random(_settings->follower.scaleTTL.x, _settings->follower.scaleTTL.y);
	_world->scaleByPath(id, &_scale_path, ttl);
	_world->lookAt(id, target, 0.0f);
	v3 r = _world->getRotation(id);
	v2 vel = math::getRadialVelocity(r.x, _settings->follower.initialVelocity);
	_world->moveBy(id, vel, ttl);
}

void FollowerBehavior::tick(const ds::ActionEvent& event, ID target, int objectType) {
	if (event.action == ds::AT_SCALE|| event.action == ds::AT_SCALE_BY_PATH) {
		_world->attachCollider(event.id, ds::PST_CIRCLE);
		_world->seek(event.id, target, _settings->follower.seekVelocity);
		_world->separate(event.id, objectType, _settings->follower.separationDistance, _settings->follower.relaxation);
	}
}
*/