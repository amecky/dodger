#pragma once
#include "..\GameContext.h"

class Player {

public:
	Player(GameContext* ctx);
	~Player();
	void move(float dt);
	void render();
	void reset();
private:
	GameContext* _context;
	bool _dying;
	v2 _cursor_pos;
};

