#pragma once
#include "Timeline.h"
#include "GameContext.h"

struct PendingEnemy {
	StaticHash type;
	v2 pos;
	float timer;
};

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
	ds::Array<PendingEnemy> _pendings;
};

