#pragma once
#include <core\world\World.h>
#include <particles\ParticleManager.h>
#include "..\GameSettings.h"
#include "..\actions\BulletTrailAction.h"

struct BulletData {
	v3 previous;
	float sqrDist;
};

class Bullets {

public:
	Bullets(ds::World* world, GameSettings* settings);
	~Bullets() {}
	void tick(float dt);
	void start(ID player);
	void stop();
	void kill(ID id);
private:
	ID _player;
	ds::World* _world;
	bool _active;
	float _timer;
	ds::ParticleManager* _particles;
	GameSettings* _settings;
};
