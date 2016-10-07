#pragma once
#include <core\world\World.h>
#include "..\GameSettings.h"

class WanderingCubes {

public:
	WanderingCubes(ds::World* world, GameSettings* settings);
	~WanderingCubes();
	void reset();
	void init(int max, float spawnDelay);
	void tick(float dt);
	void create();
private:
	ds::World* _world;
	GameSettings* _settings;
	float _timer;
	float _delay;
	int _max;
	int _emitted;
	ds::V3Path _scale_path;
};

