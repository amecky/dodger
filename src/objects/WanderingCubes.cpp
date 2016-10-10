#include "WanderingCubes.h"
#include <core\math\math.h>
#include "..\Constants.h"
// ---------------------------------------
// WanderingCubes
// ---------------------------------------
WanderingCubes::WanderingCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings) : BasicCubes(world, emitter, settings) {
	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
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
	}
	else if (event.action == ds::AT_ROTATE_BY) {
		v3 rot = _world->getRotation(event.id);
		float vel = math::random(_settings->wanderer.minVelocity, _settings->wanderer.maxVelocity);
		v3 v = math::getRadialVelocity(rot.x, vel);
		float ttl = math::random(_settings->wanderer.minMoveTTL, _settings->wanderer.maxMoveTTL);
		_world->moveBy(event.id, v, ttl);
	}
	else if (event.action == ds::AT_MOVE_BY) {
		float ttl = math::random(_settings->wanderer.minRotationTTL, _settings->wanderer.maxRotationTTL);
		_world->rotateBy(event.id, getRotationAngle(), ttl);
	}
}

// ---------------------------------------
// create
// ---------------------------------------
void WanderingCubes::create() {
	_emitter->next();
	ID id = _world->create(_emitter->get(), math::buildTexture(80, 410, 40, 40), OT_WANDERER);
	float ttl = math::random(_settings->wanderer.minScaleTTL, _settings->wanderer.maxScaleTTL);
	_world->scaleByPath(id, &_scale_path, ttl);
}

