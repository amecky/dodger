#pragma once
#include "Timeline.h"
#include "GameContext.h"


class Enemies {

public:
	Enemies(GameContext* context);
	~Enemies();
	void start();
	void tick(float dt);
private:
	GameContext* _ctx;
	Timeline _timeline;
	EmitterQueue _queue;
};

