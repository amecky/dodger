#pragma once
#include "BasicCubes.h"

class WanderingCubes : public BasicCubes {

public:
	WanderingCubes(ds::World* world, GameSettings* settings);
	~WanderingCubes();
	void handleEvents(ID target, float dt);
	void create(const CubeEmitter& emitter);
private:
	ds::V3Path _scale_path;
};

