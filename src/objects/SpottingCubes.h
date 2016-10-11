#pragma once
#include "BasicCubes.h"

class SpottingCubes : public BasicCubes {

public:
	SpottingCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings);
	~SpottingCubes();
	void onEvent(const ds::ActionEvent& event, ID target, float dt);
	void create();
	ObjectType getObjectType() const {
		return OT_SPOTTER;
	}
private:
	ds::V3Path _scale_path;
};
