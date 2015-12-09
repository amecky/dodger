#pragma once
#include <Vector.h>

struct Ball;

namespace behavior {

	void seek(Ball* balls, int count, const v2& target, float velocity, float dt);

	void separate(Ball* balls, int count, const v2& target, float minDistance, float relaxation, float dt);

	void align(Ball* balls, int count, const v2& target, float desiredDistance, float dt);

	void simple_move(Ball* balls, int count, float dt);
}