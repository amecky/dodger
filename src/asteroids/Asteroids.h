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
	void startAsteroid(int type);
	void startAsteroid(int type, const v2& pos, float angle);
	bool kill(ID id);
	v2 pickStartPoint(float minRadius);
private:	
	AsteroidDefinitions _definitions;
	GameContext* _context;
};

