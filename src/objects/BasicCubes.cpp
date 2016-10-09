#include "BasicCubes.h"



BasicCubes::BasicCubes(ds::World* world, GameSettings* settings) : _world(world), _settings(settings), _timer(0.0f), _emitted(0), _running(false), _spawnDelay(0.0f) {
}


BasicCubes::~BasicCubes() {
}

void BasicCubes::reset() {
	_timer = 0.0f;
	_emitted = 0;
	_spawnDelay = 0.0f;
	_running = false;
}

void BasicCubes::init(const SpawnSettings& spawnSettings) {
	_spawnSettings = spawnSettings;
	_spawnDelay = math::random(_spawnSettings.minDelay, _spawnSettings.maxDelay);
	_timer = 0.0f;
	_emitted = 0;
	_running = true;
}

void BasicCubes::tick(ID target, float dt) {
	if (_emitted < _spawnSettings.maxCubes && _running) {
		_timer += dt;
		if (_timer > _spawnDelay) {
			create();
			++_emitted;
			_spawnDelay = math::random(_spawnSettings.minDelay, _spawnSettings.maxDelay);
		}
	}

	handleEvents(target, dt);
}