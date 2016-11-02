#pragma once
#include <gamestates\GameState.h>
#include <renderer\render_types.h>
#include <imgui\IMGUI.h>
#include "..\GameContext.h"
#include "..\EventBuffer.h"
#include <core\lib\DataArray.h>
#include <core\world\World.h>
#include <particles\ParticleManager.h>
#include "..\objects\Bullets.h"
#include "..\Levels.h"
#include "AsteroidDefinitions.h"

struct AsteroidData {
	int energy;
	int type;
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
	void startAsteroid(int type, const v2& pos,float angle);
	void splitAsteroid(ID id);

	GameContext* _context;
	ID _player;
	v2 _playerPrevious;
	float _playerAngle;
	ID _cursor;
	ds::GUIDialog* _hud;
	Bullets* _bullets;
	int _kills;
	v2 _cursor_pos;
	AsteroidDefinitions _definitions;
};

