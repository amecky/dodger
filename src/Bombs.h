#pragma once
#include <sprites\SpriteBatch.h>
#include "GameContext.h"
#include <lib\DataArray.h>
#include "EventBuffer.h"
#include <utils\TimedObject.h>

struct Bomb : public ds::ObjectTimer {

	enum BombState {
		BS_STARTING,
		BS_ACTIVE,
		BS_TICKING,
		BS_FOLLOWING
	};

	ID id;
	v2 position;
	v2 scale;
	BombState state;
	float rotation;
	v2 velocity;
	ds::Color color;

	Bomb() : position(640, 360), scale(1, 1), state(BS_ACTIVE) {}

};

class Bombs {

public:
	Bombs(GameContext* context);
	~Bombs();
	void activate(int maxCurrent);
	void tick(EventBuffer* buffer,float dt);
	void render();
	void killAll();
	bool grab(const v2& pos, float radius,ID* id);
	bool contains(ID id) const;
	void follow(ID id,const v2& target);
	void burst(ID id, float direction);
	const v2& getPosition(ID id) const;
	void clear();
private:
	void checkInterception(EventBuffer* buffer, const v2& pos, float radius);
	void create();
	void scaleBombs(EventBuffer* buffer, float dt);
	void drawRing(const v2& pos,float timer);
	GameContext* _context;
	int _max_current;
	float _spawn_timer;
	ds::DataArray<Bomb, 32> _bombs;
	float _cells[36];
	ds::Texture _texture;
	ds::Texture _ring_texture;

};

