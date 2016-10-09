#pragma once
#include <core\world\World.h>
#include "..\GameSettings.h"

class CubeEmitter {

public:
	virtual v2 next() const = 0;
};

class TestCubeEmitter : public CubeEmitter {

public:
	v2 next() const {
		return v2(512, 384);
	}
};

class RandomCubeEmitter : public CubeEmitter {

public:
	v2 next() const {
		float x = math::random(150.0f, 900.0f);
		float y = math::random(150.0f, 600.0f);
		return v2(x, y);
	}
};


struct SpawnSettings {
	float minDelay;
	float maxDelay;
	int maxCubes;
	int emittCubes;
};

class BasicCubes {

public:
	BasicCubes(ds::World* world, GameSettings* settings);
	~BasicCubes();
	void reset();
	void init(const SpawnSettings& spawnSettings);
	void tick(ID target, float dt);
	virtual void create(const CubeEmitter& emitter) = 0;
	virtual void handleEvents(ID target, float dt) = 0;
protected:
	ds::World* _world;
	GameSettings* _settings;
	float _timer;
	float _spawnDelay;
	int _emitted;
	bool _running;
	SpawnSettings _spawnSettings;
};

