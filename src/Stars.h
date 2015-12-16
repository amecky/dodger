#pragma once
#include <lib\DataArray.h>
#include <sprites\SpriteBatch.h>
#include "GameContext.h"

class Stars {

struct Star : ds::BasicSprite {

	float timer;

	Star() : ds::BasicSprite(), timer(0.0f) {}
};

public:
	Stars(GameContext* context);
	~Stars();
	void add(const v2& position,int count = 1);
	void render();
	void tick(float dt);
	void move(const v2& target, float dt);
	int pickup(const v2& target, float radius);
	void clear();
private:
	ds::DataArray<Star, 128> _stars;
	GameContext* _context;
	ds::FloatPath _scale_path;
};

