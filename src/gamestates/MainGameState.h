#pragma once
#include <gamestates\GameState.h>
#include <renderer\render_types.h>
#include <imgui\IMGUI.h>
#include "..\GameContext.h"
#include <core\lib\DataArray.h>
#include <core\world\World.h>
#include <particles\ParticleManager.h>
#include "..\objects\Bullets.h"
#include "..\Enemies.h"

struct GameTimer {

	float timer;
	int seconds;
	int total_seconds;

	GameTimer() : timer(0.0f), seconds(0) , total_seconds(0) {}

	bool tick(float dt) {
		timer += dt;
		if (timer >= 1.0f) {
			timer -= 1.0f;
			--seconds;
			++total_seconds;
			if (seconds <= 0) {
				return true;
			}
		}
		return false;
	}

	void reset(int sec) {
		seconds = sec;
		timer = 0.0f;
		total_seconds = 0;
	}
};

class MainGameState : public ds::GameState {

public:
	MainGameState(GameContext* context);
	virtual ~MainGameState();
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
	ID _player;
	ID _playerRing;
	v2 _playerPrevious;
	float _playerAngle;
	ID _cursor;
	ds::GUIDialog* _hud;
	Bullets* _bullets;
	int _kills;
	v2 _cursor_pos;
	bool _dying;
	float _dying_timer;
	bool _levelRunning;
	int _level;
	Enemies* _enemies;
};

