#include "WanderingCubes.h"
#include <core\math\math.h>
#include "..\Constants.h"
// ---------------------------------------
// WanderingCubes
// ---------------------------------------
WanderingCubes::WanderingCubes(ds::World* world,GameSettings* settings) : BasicCubes(world,settings) {
	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
}

WanderingCubes::~WanderingCubes() {
}

// ---------------------------------------
// handle events
// ---------------------------------------
void WanderingCubes::handleEvents(ID target, float dt) {
	if (_world->hasEvents()) {
		uint32_t n = _world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _world->getEvent(i);
			int type = _world->getType(event.id);
			if ( type == OT_WANDERER) {
				if (event.action == ds::AT_SCALE_BY_PATH) {
					float angle = math::random(_settings->wanderer.minRotationAngle, _settings->wanderer.maxRotationAngle);
					float ttl = math::random(_settings->wanderer.minRotationTTL, _settings->wanderer.maxRotationTTL);
					_world->rotateBy(event.id, DEGTORAD(angle), ttl);
				}
				else if (event.action == ds::AT_ROTATE_BY) {
					v3 rot = _world->getRotation(event.id);
					float vel = math::random(_settings->wanderer.minVelocity, _settings->wanderer.maxVelocity);
					v3 v = math::getRadialVelocity(rot.x, vel);
					float ttl = math::random(_settings->wanderer.minMoveTTL, _settings->wanderer.maxMoveTTL);
					_world->moveBy(event.id, v, ttl);
				}
				else if (event.action == ds::AT_MOVE_BY) {
					float angle = math::random(_settings->wanderer.minRotationAngle, _settings->wanderer.maxRotationAngle);
					float ttl = math::random(_settings->wanderer.minRotationTTL, _settings->wanderer.maxRotationTTL);
					_world->rotateBy(event.id, DEGTORAD(angle), ttl);
				}
			}
		}
	}
}

// ---------------------------------------
// create
// ---------------------------------------
void WanderingCubes::create(const CubeEmitter& emitter) {
	ID id = _world->create(emitter.next(), math::buildTexture(80, 410, 40, 40), OT_WANDERER);
	float ttl = math::random(_settings->wanderer.minScaleTTL, _settings->wanderer.maxScaleTTL);
	_world->scaleByPath(id, &_scale_path, ttl);
}

