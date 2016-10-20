#pragma once
#include "objects\BasicCubes.h"

enum StageActorMode {
	SAM_STARTED,
	SAM_RUNNING
};

struct StageActor {
	BehaviorFunc behavior;
	CubeEmitter* emitter;
	StaticHash templateName;
	int num;
	int emitted;
	int total;
	float delay;
	float initialDelay;
	StageActorMode mode;
};

struct LevelItem {
	int cubeType;
	int num;
	int emitt;
	int emitter;
	float delay;
};

struct Level {
	LevelItem items[10];
	int num;
};

class Levels {

public:
	Levels();
	~Levels();
	void load();
	const Level& get(int index) const;
	int getNumberOfItems(int index) const;
	void start(int level);
	StageActor* create(int level, int offset);
private:
	Level _levels[16];
	ds::Array<StageActor*> _actors;
};

