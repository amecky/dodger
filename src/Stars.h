#pragma once
#include <lib\DataArray.h>
#include <sprites\SpriteBatch.h>
#include "GameContext.h"

class Stars {

struct Star {
	ID id;
	v2 position;
	v2 scale;
	float timer;
};

public:
	Stars(GameContext* context);
	~Stars();
	void add(const v2& position);
	void render();
	void tick(float dt);
	void move(const v2& target, float dt);
	int pickup(const v2& target, float radius);
	void clear();
private:
	ds::DataArray<Star, 128> _stars;
	GameContext* _context;
};

