#pragma once
#include <sprites\SpriteBatch.h>
#include "GameContext.h"
#include <lib\DataArray.h>
#include "EventBuffer.h"
#include <utils\TimedObject.h>

struct Bomb : public ds::ObjectTimer , ds::BasicSprite {

	enum BombState {
		BS_STARTING,
		BS_ACTIVE,
		BS_TICKING,
		BS_FOLLOWING
	};

	BombState state;
	v2 velocity;

	Bomb() : BasicSprite() , state(BS_ACTIVE) , velocity(0,0) {}

};

class Bombs {

typedef ds::DataArray<Bomb, 32> BombArray;

public:
	Bombs(GameContext* context);
	~Bombs();
	void activate();
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
	float _spawn_timer;
	BombArray _bombs;
	float _cells[36];
	ds::Texture _texture;
	ds::Texture _ring_texture;
	ds::Vector2fPath _scale_path;

};

