#pragma once
#include <core\world\World.h>
#include "GameSettings.h"

namespace behaviors {

	void createBasicBehaviors(ds::World* world, ds::V3Path *path);

	void createWanderer(ds::World* world);

	void createSpotterBehavior(ds::World* world);

	void createBulletBehavior(ds::World* world, float velocity);

	void createFollowerBehavior(ds::World* world, ds::V3Path *path, GameSettings* settings);

	void createWiggleFollowerBehavior(ds::World* world, ds::V3Path *path, GameSettings* settings);

}
