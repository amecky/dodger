#include "BasicCubes.h"

// -------------------------------------------------
// BasicCubes
// -------------------------------------------------
BasicCubes::BasicCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings) : _world(world), _emitter(emitter), _settings(settings), _timer(0.0f), _emitted(0), _running(false), _spawnDelay(0.0f) {
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
			create();
			++_emitted;
			_spawnDelay = math::random(_spawnSettings.minDelay, _spawnSettings.maxDelay);
		}
	}

	if (_world->hasEvents()) {
		uint32_t n = _world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _world->getEvent(i);
			int type = _world->getType(event.id);
			if (type == getObjectType()) {
				onEvent(event, target, dt);
			}
		}
	}
}