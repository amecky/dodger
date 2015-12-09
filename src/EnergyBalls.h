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
	EBT_EOL
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
};

class EnergyBalls {

public:
	EnergyBalls(GameContext* context);
	~EnergyBalls();
	void activate();
	void render();
	void tick(float dt);
	int killBalls(const v2& bombPos,v2* positions);
	bool checkBallsInterception() const;
	void killAll();
	void emitt(EnergyBallType type, int count);
private:
	void move(float dt);
	void createBall(const v2& pos);
	void createBall(const v2& pos,int current, int total, EnergyBallType type);
	void scaleGrowingBalls(float dt);
	void moveBalls(float dt);
	
	
	GameContext* _context;
	ds::DataArray<Ball, MAX_BALLS> _balls;
	v2 _spawner_position;

	SpawnerData _spawnData;
	float _spawn_timer;
	float _spawn_delay;
	float _big_ball_timer;
	StartPoints _startPoints;
	BallEmitter* _emitter;
	int _counter;
	int _maxBalls;
	LevelData _level_data;
};

