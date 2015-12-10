#pragma once
#include <sprites\SpriteBatch.h>
#include "GameContext.h"
#include <lib\DataArray.h>
#include "BallEmitter.h"
#include "Stars.h"
#include "Behaviors.h"

enum EnergyBallType {
	EBT_FOLLOWER,
	EBT_BIG_CUBE,
	EBT_HUGE_CUBE,
	EBT_EOL
};

struct KilledBall {

	v2 position;
	EnergyBallType type;

};

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
	v2 force;
	v2 scale;
	ds::Texture texture;
	float size;
	float rotation;
	BallState state;
	int behaviors;
	float timer;
	ds::Color color;
	EnergyBallType type;

};

struct LevelData {
	int totalBalls;
	int emittBalls;
	int minBalls;
	int spawnBalls;
	int emitted;
	float bigBallEmittTime;
	float hugeBallEmittTime;
	float ballEmittTime;
};

class EnergyBalls {

public:
	EnergyBalls(GameContext* context);
	~EnergyBalls();
	void activate();
	void render();
	void tick(float dt);
	int killBalls(const v2& bombPos, KilledBall* positions);
	bool checkBallsInterception() const;
	void killAll();
	void emitt(EnergyBallType type, int count);
private:
	void move(float dt);
	void createBall(const v2& pos,int current, int total, EnergyBallType type);
	void scaleGrowingBalls(float dt);
	void moveBalls(float dt);
	bool buildFromTemplate(Ball* ball, const char* name);
	
	GameContext* _context;
	ds::DataArray<Ball, MAX_BALLS> _balls;
	v2 _spawner_position;

	SpawnerData _spawnData;
	float _spawn_timer;
	float _spawn_delay;
	float _ball_timer;
	float _big_ball_timer;
	float _huge_ball_timer;
	StartPoints _startPoints;
	BallEmitter* _emitter;
	int _counter;
	int _maxBalls;
	LevelData _level_data;
	int _killed;
};

