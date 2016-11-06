#pragma once
#include "AsteroidDefinitions.h"
#include "..\GameContext.h"

struct AsteroidData {
	int energy;
	int type;
	float startAngle;
};


class Asteroids {

public:
	Asteroids(GameContext* context);
	~Asteroids();
	void splitAsteroid(ID id);
	void startAsteroid(int type);
	void startAsteroid(int type, const v2& pos, float angle);
	bool kill(ID id);
	void handleEvent(const ds::ActionEvent& event);
	const AsteroidInfo& getInfo(ID id) const;
private:	
	AsteroidDefinitions _definitions;
	GameContext* _context;
	ds::V3Path _scale_path;
};

