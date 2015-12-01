#pragma once
#include <sprites\SpriteBatch.h>
#include "GameContext.h"
#include <lib\DataArray.h>

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
	BallState state;
	float timer;

};

// ----------------------------------------
// Gate
// ----------------------------------------
struct Gate {

	enum Orientation {
		VERTICAL,
		HORIZONTAL
	};

	enum State {
		ACTIVE,
		INACTIVE,
		FLASHING
	};
	ID id;
	v2 position;
	Orientation orientation;
	State state;
	float timer;
	ds::AABBox aabBox;
	v2 scale;

	Gate() : position(512, 384), orientation(Gate::VERTICAL), state(Gate::INACTIVE), timer(0.0f) , scale(1,1) {}

};


class Behavior {

public:
	virtual void tick(Ball* balls, int count, const v2& target,float dt) = 0;
};

class SeekBehavior : public Behavior {

public:
	void tick(Ball* balls, int count, const v2& target, float dt);

};

class SeparationBehavior : public Behavior {

public:
	void tick(Ball* balls, int count, const v2& target, float dt);

};

class AlignBehavior : public Behavior {

public:
	void tick(Ball* balls, int count, const v2& target, float dt);

};

class EnergyBalls {

public:
	EnergyBalls(GameContext* context);
	~EnergyBalls();
	void render();
	void move(float dt);

private:
	void createGate(const v2& pos,Gate::Orientation orientation = Gate::VERTICAL);
	void createBall(const v2& pos);
	void scaleGrowingBalls(float dt);
	void moveStartingBalls(float dt);
	void moveBalls(float dt);
	void scaleGates(float dt);
	void checkGateInterception();

	GameContext* _context;
	ds::DataArray<Ball,256> _balls;
	ds::DataArray<Gate,16> _gates;
	SeekBehavior _seek;
	SeparationBehavior _separation;
	AlignBehavior _align;
};

