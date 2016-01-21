#pragma once
#include <sprites\SpriteBatch.h>
#include "GameContext.h"
#include <lib\DataArray.h>
#include "EventBuffer.h"
#include <utils\TimedObject.h>

struct BombData {

	enum BombState {
		BS_STARTING,
		BS_ACTIVE,
		BS_TICKING,
		BS_FOLLOWING
	};
	BombState state;
	float timer;

	BombData() : state(BS_ACTIVE) , timer(0.0f) {}

};

class Bombs {

public:
	Bombs(GameContext* context);
	~Bombs();
	void activate();
	void tick(EventBuffer* buffer,float dt);
	void render();
	void killAll();
	bool grab(const v2& pos, float radius,ds::SID* id);
	void follow(ds::SID id,const v2& target);
	void burst(ds::SID id, float direction);
	void clear();
	void handleEvents(const ds::ActionEventBuffer& buffer);
private:
	void checkInterception(EventBuffer* buffer, const v2& pos, float radius);
	void create();
	void scaleBombs(EventBuffer* buffer, float dt);
	void drawRing(const v2& pos,float timer);
	GameContext* _context;
	ds::World* _world;
	float _spawn_timer;
	ds::SID _bomb_sids[16];
	float _cells[36];
	ds::Texture _ring_texture;
	ds::Vector2fPath _scale_path;

};

