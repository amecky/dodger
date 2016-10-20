#include "WanderingCubes.h"
#include <core\math\math.h>
#include "..\Constants.h"
// ---------------------------------------
// WanderingCubes
// ---------------------------------------
WanderingCubes::WanderingCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings) : BasicCubes(world, emitter, settings) {	
}

WanderingCubes::~WanderingCubes() {
}

float WanderingCubes::getRotationAngle() {
	float angle = math::random(_settings->wanderer.minRotationAngle, _settings->wanderer.maxRotationAngle);
	int dir = math::random(0, 100);
	if (dir >= 50) {
		angle *= -1.0f;
	}
	return DEGTORAD(angle);
}
// ---------------------------------------
// handle events
// ---------------------------------------
void WanderingCubes::onEvent(const ds::ActionEvent& event, ID target, float dt) {
	if (event.action == ds::AT_SCALE_BY_PATH) {
		float ttl = math::random(_settings->wanderer.minRotationTTL, _settings->wanderer.maxRotationTTL);
		_world->rotateBy(event.id, getRotationAngle(), ttl);
		_world->attachCollider(event.id, ds::PST_CIRCLE, v2(44.0f, 44.0f));
	}
	else if (event.action == ds::AT_ROTATE_BY) {
		v3 rot = _world->getRotation(event.id);
		float vel = math::random(_settings->wanderer.minVelocity, _settings->wanderer.maxVelocity);
		v3 v = math::getRadialVelocity(rot.x, vel);
		float ttl = math::random(_settings->wanderer.minMoveTTL, _settings->wanderer.maxMoveTTL);
		_world->moveBy(event.id, v, ttl);
		_world->scale(event.id, v3(1.0f, 1.0f, 1.0f), v3(0.9f, 0.9f, 0.9f), 0.2f);
	}
	else if (event.action == ds::AT_MOVE_BY) {
		float ttl = math::random(_settings->wanderer.minRotationTTL, _settings->wanderer.maxRotationTTL);
		_world->rotateBy(event.id, getRotationAngle(), ttl);
		_world->scale(event.id, v3(0.9f, 0.9f, 0.9f), v3(1.0f, 1.0f, 1.0f), 0.2f);
	}
}

// ---------------------------------------
// create
// ---------------------------------------
void WanderingCubes::create(ID target, int num) {
	createCubes(SID("Wanderer"), target, num, 5);	
}

