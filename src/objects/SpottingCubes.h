#pragma once
#include "BasicCubes.h"

class SpottingCubes : public BasicCubes {

public:
	SpottingCubes(ds::World* world, GameSettings* settings);
	~SpottingCubes();
	void handleEvents(ID target, float dt);
	void create(const CubeEmitter& emitter);
private:
	ds::V3Path _scale_path;
};

