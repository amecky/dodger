#pragma once
#include <core\world\World.h>

struct WanderingCubeData {

	enum WanderingCubeState {
		WCS_STARTING,
		WCS_ROTATING,
		WCS_MOVING,
		WCS_IDLE
	};
	WanderingCubeState state;
	float timer;
	v3 velocity;

	WanderingCubeData() : state(WCS_IDLE), timer(0.0f) , velocity(0.0f) {}

};

class WanderingCubes {

public:
	WanderingCubes(ds::World* world);
	~WanderingCubes();
	void reset();
	void init(int max, float spawnDelay);
	void tick(float dt);
	void create();
private:
	ds::World* _world;
	float _timer;
	float _delay;
	int _max;
	int _emitted;
	ds::V3Path _scale_path;
};

