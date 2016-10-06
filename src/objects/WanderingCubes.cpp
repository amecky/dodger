#include "WanderingCubes.h"
#include <core\math\math.h>


WanderingCubes::WanderingCubes(ds::World* world) : _world(world) , _timer(0.0f) , _delay(0.0f) , _max(0) , _emitted(0) {
	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
}

WanderingCubes::~WanderingCubes() {
}

void WanderingCubes::reset() {
}

void WanderingCubes::init(int max, float spawnDelay) {
	_delay = spawnDelay;
	_max = max;
	_timer = 0.0f;
	_emitted = 0;
}

void WanderingCubes::tick(float dt) {
	if (_emitted < _max) {
		_timer += dt;
		if (_timer > _delay) {
			// emitt ball
		}
	}

	if (_world->hasEvents()) {
		uint32_t n = _world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _world->getEvent(i);
			int type = _world->getType(event.id);
			if ( type == 30 ) {
				if (event.action == ds::AT_SCALE_BY_PATH) {
					WanderingCubeData* data = (WanderingCubeData*)_world->get_data(event.id);
					data->state = WanderingCubeData::WCS_ROTATING;
					float angle = math::random(45.0f, 180.0f);
					_world->rotateBy(event.id, DEGTORAD(angle), 1.0f);
				}
				if (event.action == ds::AT_ROTATE_BY) {
					WanderingCubeData* data = (WanderingCubeData*)_world->get_data(event.id);
					data->state = WanderingCubeData::WCS_MOVING;
					v3 rot = _world->getRotation(event.id);
					v3 v = math::getRadialVelocity(rot.x, 50.0f);
					_world->moveBy(event.id, v);
				}
			}
		}
	}
}

void WanderingCubes::create() {
	float x = math::random(200.0f, 800.0f);
	float y = math::random(200.0f, 550.0f);
	ID id = _world->create(v2(x, y), math::buildTexture(40, 280, 40, 40), 30);
	WanderingCubeData* data = (WanderingCubeData*)_world->attach_data(id, sizeof(WanderingCubeData), 1);
	data->state = WanderingCubeData::WCS_STARTING;
	_world->scaleByPath(id, &_scale_path, 0.8f);
}

