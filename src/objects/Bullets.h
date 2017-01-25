#pragma once
#include "..\GameSettings.h"
#include "..\GameContext.h"
#include <core\base\GameObject.h>

struct BulletData {
	v3 previous;
	float sqrDist;
};

class Bullets : public ds::GameObject {

public:
	Bullets(GameContext* context);
	~Bullets() {}
	void tick(float dt);
	void start();
	void stop();
	void kill(ID id);
	void killAll();
private:
	void create(float offset);
	GameContext* _context;
	SimplePlayer* _player;
	bool _shooting;
	float _timer;
};
