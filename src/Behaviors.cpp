#include "Behaviors.h"
#include "EnergyBalls.h"
#include <math\Bitset.h>
// ------------------------------------------------
// behavior
// ------------------------------------------------
namespace behavior {

	void simple_move(Ball* balls, int count, float dt) {
		for (int i = 0; i < count; ++i) {
			if (ds::bit::is_set(balls[i].behaviors, SIMPLE_MOVE_BIT)) {
				balls[i].force = balls[i].velocity;
			}
		}
	}
	// ------------------------------------------------
	// seek
	// ------------------------------------------------
	void seek(Ball* balls, int count, const v2& target, float velocity, float dt) {
		for (int i = 0; i < count; ++i) {
			if (ds::bit::is_set(balls[i].behaviors, SEEK_BIT)) {
				if (balls[i].state == Ball::BS_MOVING) {
					v2 diff = target - balls[i].position;
					v2 n = normalize(diff);
					v2 desired = n * velocity;
					balls[i].force += desired;
				}
			}
		}
	}

	// ------------------------------------------------
	// separate
	// ------------------------------------------------
	void separate(Ball* balls, int count, const v2& target, float minDistance, float relaxation, float dt) {
		float sqrDist = minDistance * minDistance;
		for (int i = 0; i < count; ++i) {
			Ball& ball = balls[i];
			if (ds::bit::is_set(ball.behaviors, SEPARATE_BIT)) {
				if (ball.state == Ball::BS_MOVING) {
					int cnt = 0;
					v2 separationForce = v2(0, 0);
					v2 averageDirection = v2(0, 0);
					v2 distance = v2(0, 0);
					for (int j = 0; j < count; j++) {
						if (i != j) {
							v2 dist = balls[j].position - ball.position;
							if (sqr_length(dist) < sqrDist) {
								++cnt;
								separationForce += dist;
								separationForce = normalize(separationForce);
								separationForce = separationForce * relaxation;
								averageDirection += separationForce;
							}
						}
					}
					if (cnt > 0) {
						balls[i].force -= averageDirection;
					}
				}
			}
		}
	}

	// ------------------------------------------------
	// align
	// ------------------------------------------------
	void align(Ball* balls, int count, const v2& target, float desiredDistance, float dt) {
		float sqrDesired = desiredDistance * desiredDistance;
		for (int i = 0; i < count; ++i) {
			Ball& ball = balls[i];
			if (ds::bit::is_set(ball.behaviors, ALIGN_BIT)) {
				if (ball.state == Ball::BS_MOVING) {
					int cnt = 0;
					v2 separationForce = v2(0, 0);
					v2 averageDirection = v2(0, 0);
					v2 distance = v2(0, 0);
					for (int j = 0; j < count; j++) {
						if (i != j) {
							v2 dist = balls[j].position - ball.position;
							if (sqr_length(dist) < sqrDesired) {
								++cnt;
								averageDirection += ball.velocity;
							}
						}
					}
					if (cnt > 0) {
						averageDirection /= static_cast<float>(cnt);
						balls[i].force += averageDirection;
					}
				}
			}
		}
	}

}