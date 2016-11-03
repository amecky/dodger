#pragma once
#include "AsteroidDefinitions.h"
#include "..\GameContext.h"

struct AsteroidData {
	int energy;
	int type;
};


class Asteroids {

public:
	Asteroids(GameContext* context);
	~Asteroids();
	void splitAsteroid(ID id);
	void startAsteroid(int type, const v2& pos, float angle);
	bool kill(ID id);
private:
	AsteroidDefinitions _definitions;
	GameContext* _context;
};

