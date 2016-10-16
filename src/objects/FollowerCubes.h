#pragma once
#include "BasicCubes.h"

class FollowerCubes : public BasicCubes {

public:
	FollowerCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings);
	~FollowerCubes();
	void onEvent(const ds::ActionEvent& event, ID target, float dt);
	void create(ID target);
	ObjectType getObjectType() const {
		return OT_FOLLOWER;
	}
private:
};

