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
struct Ball : ds::BasicSprite {

	enum BallState {
		BS_GROWING,
		BS_STARTING,
		BS_MOVING,
		BS_EOL
	};
	v2 velocity;
	v2 force;
	float size;
	BallState state;
	int behaviors;
	float timer;
	EnergyBallType type;

	Ball() : ds::BasicSprite(), velocity(0, 0), force(0, 0), size(1.0f), state(BS_EOL), behaviors(0), timer(0.0f), type(EBT_EOL) {}

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
	int _active_balls;

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
	int _killed;
	int _emitted;
};

