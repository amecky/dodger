#pragma once
#include "BasicCubes.h"

class WanderingCubes : public BasicCubes {

public:
	WanderingCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings);
	~WanderingCubes();
	void create(ID target);
	void onEvent(const ds::ActionEvent& event, ID target, float dt);
	ObjectType getObjectType() const {
		return OT_WANDERER;
	}
private:
	float getRotationAngle();
};

