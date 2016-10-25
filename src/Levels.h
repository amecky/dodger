#pragma once
#include "objects\CubeEmitter.h"
#include "objects\CubeBehavior.h"

// ---------------------------------------
// Cube data
// ---------------------------------------
struct CubeData {
	int energy;
};

enum StageActorMode {
	SAM_STARTED,
	SAM_RUNNING
};

// ---------------------------------------
// Stage actor
// ---------------------------------------
struct StageActor {
	CubeBehavior* behavior;
	CubeEmitter* emitter;
	StaticHash templateName;
	int num;
	int emitted;
	int total;
	float delay;
	float initialDelay;
	StageActorMode mode;
	float timer;
	int energy;
	int type;
};

// ---------------------------------------
// Level item
// ---------------------------------------
struct LevelItem {
	int cubeType;
	int num;
	int emitt;
	int emitter;
	float delay;
	float initialDelay;
	int energy;
	int behavior;
};

// ---------------------------------------
// Level
// ---------------------------------------
struct Level {
	LevelItem items[10];
	int num;
};

// ---------------------------------------
// Levels
// ---------------------------------------
class Levels {

public:
	Levels(ds::World* world, GameSettings* settings);
	~Levels();
	void load();
	const Level& get(int index) const;
	int getNumberOfItems(int index) const;
	void start(int level);
	StageActor* create(int level, int offset);
	void tick(ID target, float dt);
	int getNumberToKill() const {
		return _total;
	}
	bool isActive() const {
		return _active;
	}
	void killAll();
private:
	Level _levels[16];
	ds::Array<StageActor*> _actors;
	float _timer;
	bool _active;
	int _total;
	bool _emitting;
	ds::Array<CubeEmitter*> _emitters;
	ds::Array<CubeBehavior*> _behaviors;
	ds::World* _world;
	ds::V3Path _scale_path;
};

