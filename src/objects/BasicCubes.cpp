#include "BasicCubes.h"


v2 CircleCubeEmitter::get(int index, int total) {
	if (index == 0) {
		return _position;
	}
	++_cnt;
	if (_cnt > _threshold) {
		_threshold += 4;
		++_ring;
		_cnt = 0;
		_offset = math::random(0.0f, DEGTORAD(45.0f));
	}
	float step = TWO_PI / _threshold;
	float angle = step * static_cast<float>(index % _threshold) + math::random(DEGTORAD(-15.0f), DEGTORAD(15.0f));
	float r = _radius * _ring;
	return _position + math::getRadialVelocity(angle, _radius * _ring);
}

v2 LineCubeEmitter::get(int index, int total) {
	v2 p = _position;
	if (_vertical) {
		p.x += index * _distance;
	}
	else {
		p.y += index * _distance;
	}
	return p;
}
// -------------------------------------------------
// BasicCubes
// -------------------------------------------------
BasicCubes::BasicCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings) : _world(world), _emitter(emitter), _settings(settings), _timer(0.0f), _emitted(0), _running(false), _spawnDelay(0.0f) {
	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
}

BasicCubes::~BasicCubes() {
}

// -------------------------------------------------
// reset
// -------------------------------------------------
void BasicCubes::reset() {
	_timer = 0.0f;
	_emitted = 0;
	_spawnDelay = 0.0f;
	_running = false;
}

// -------------------------------------------------
// init
// -------------------------------------------------
void BasicCubes::init(const SpawnSettings& spawnSettings) {
	_spawnSettings = spawnSettings;
	_spawnDelay = math::random(_spawnSettings.minDelay, _spawnSettings.maxDelay);
	_timer = 0.0f;
	_emitted = 0;
	_running = true;
}

// -------------------------------------------------
// tick
// -------------------------------------------------
void BasicCubes::tick(ID target, float dt) {
	if (_emitted < _spawnSettings.maxCubes && _running) {
		_timer += dt;
		if (_timer > _spawnDelay) {
			create(target, _spawnSettings.emittCubes);
			++_emitted;
			_spawnDelay = math::random(_spawnSettings.minDelay, _spawnSettings.maxDelay);
		}
	}

	if (_world->hasEvents()) {
		uint32_t n = _world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _world->getEvent(i);
			if (_world->contains(event.id)) {
				int type = _world->getType(event.id);
				if (type == getObjectType()) {
					onEvent(event, target, dt);
				}
			}
		}
	}
}

// -------------------------------------------------
// rotateTo
// -------------------------------------------------
void BasicCubes::rotateTo(ID id, ID target) {
	v3 p = _world->getPosition(id);
	v3 t = _world->getPosition(target);
	float rotation = math::getAngle(p.xy(), t.xy());
	_world->setRotation(id, rotation);
}