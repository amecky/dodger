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
};

// -------------------------------------------------------
// Enemy
// -------------------------------------------------------
struct Enemy {
	ID id;
	//EnemyBehavior* behavior;
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
	void handleEvents(ID target);
	int findEnemy(ID id);
	GameContext* _ctx;
	Timeline _timeline;
	EmitterQueue _queue;
	ds::Array<PendingEnemy> _pendings;
	std::map<StaticHash, EnemyBehavior*> _behaviorMap;
	ds::Array<Enemy> _enemies;
};

