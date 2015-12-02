#pragma once
#include <sprites\SpriteBatch.h>
#include "GameContext.h"
#include <lib\DataArray.h>
#include "BallEmitter.h"
#include "Stars.h"

// ----------------------------------------
// Ball
// ----------------------------------------
struct Ball {

	enum BallState {
		BS_GROWING,
		BS_STARTING,
		BS_MOVING,
		BS_EOL
	};
	ID id;
	v2 position;
	v2 velocity;
	v2 scale;
	float rotation;
	BallState state;
	int behaviors;
	float timer;

};

namespace behavior {

	void seek(Ball* balls, int count, const v2& target, float dt);

	void separate(Ball* balls, int count, const v2& target, float dt);

	void align(Ball* balls, int count, const v2& target, float dt);
}

struct LevelData {
	int totalBalls;
	int emittBalls;
	int minBalls;
	int spawnBalls;
	int emitted;
};

class EnergyBalls {

public:
	EnergyBalls(GameContext* context);
	~EnergyBalls();
	void activate();
	void render();
	void tick(float dt);
	int killBalls(const v2& bombPos);
private:
	void move(float dt);
	void createBall(const v2& pos);
	void scaleGrowingBalls(float dt);
	void moveStartingBalls(float dt);
	void moveBalls(float dt);
	void checkBallsInterception();
	
	GameContext* _context;
	ds::DataArray<Ball,256> _balls;
	v2 _spawner_position;

	SpawnerData _spawnData;
	float _spawn_timer;
	float _spawn_delay;
	StartPoints _startPoints;
	BallEmitter* _emitter;
	int _counter;
	int _maxBalls;
	Stars* _stars;
	LevelData _level_data;
};

