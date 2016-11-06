#pragma once
#include <gamestates\GameState.h>
#include <renderer\render_types.h>
#include <imgui\IMGUI.h>
#include "..\GameContext.h"
#include <core\lib\DataArray.h>
#include <core\world\World.h>
#include <particles\ParticleManager.h>
#include "..\objects\Bullets.h"
#include "..\Levels.h"
#include "Asteroids.h"
#include "Stages.h"

struct Player {
	ID id;
	v2 previous;
	float angle;
};

class AsteroidState : public ds::GameState {

public:
	AsteroidState(GameContext* context);
	virtual ~AsteroidState();
	void init();
	int update(float dt);
	void render();
	void activate();
	void deactivate();
	int onChar(int ascii);
	int onButtonUp(int button, int x, int y);
	int onButtonDown(int button, int x, int y);
private:
	void killPlayer();
	void movePlayer(float dt);
	bool handleCollisions(float dt);
	bool killEnemy(const ds::Collision& c, int objectType);

	GameContext* _context;
	Player _player;
	ID _cursor;
	ds::GUIDialog* _hud;
	Bullets* _bullets;
	int _kills;
	v2 _cursor_pos;
	Asteroids* _asteroids;
	Stages _stages;
	int _activeStage;
	
};

