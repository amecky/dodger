#pragma once
#include "..\GameContext.h"

class Player {

public:
	Player(GameContext* ctx);
	~Player();
	void move(float dt);
	void activate();
	void kill();
private:
	GameContext* _context;
	bool _dying;
	ID _player;
	ID _playerRing;
	v2 _playerPrevious;
	float _playerAngle;
	ID _cursor;
};

