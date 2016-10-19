#pragma once

struct LevelItem {
	int cubeType;
	int num;
	int emitt;
	int emitter;
	float delay;
};

struct Level {
	LevelItem items[3];
};

class Levels {

public:
	Levels();
	~Levels();
	void load();
	const Level& get(int index);
private:
	Level _levels[16];
};

