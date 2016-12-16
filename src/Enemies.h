#pragma once
#include "Timeline.h"
#include "GameContext.h"
#include "EnemyBehavior.h"
#include <map>

// -------------------------------------------------------
// PendingEnemy
// -------------------------------------------------------
struct PendingEnemy {
	StaticHash type;
	v2 pos;
	float timer;
	StaticHash behavior;
};

// -------------------------------------------------------
// Enemy
// -------------------------------------------------------
struct Enemy {
	ID id;
	int energy;
};

// -------------------------------------------------------
// Enemies
// -------------------------------------------------------
class Enemies {

public:
	Enemies(GameContext* context);
	~Enemies();
	void start();
	void tick(ID target, float dt);
	bool kill(ID id);
private:
	void processPendingEnemies(ID target, float dt);
	void processQueue(float dt);
	int findEnemy(ID id);
	GameContext* _ctx;
	Timeline _timeline;
	EmitterQueue _queue;
	ds::Array<PendingEnemy> _pendings;
	ds::Array<Enemy> _enemies;
};

