#pragma once
#include <gamestates\GameState.h>
#include <renderer\render_types.h>
#include <imgui\IMGUI.h>
#include "..\GameContext.h"
#include "..\objects\WanderingCubes.h"
#include "..\objects\SpottingCubes.h"
#include "..\objects\Bombs.h"
#include "..\EventBuffer.h"
#include "..\utils\Numbers.h"
#include <core\lib\DataArray.h>
#include <core\world\World.h>

struct GameTimer {

	float timer;
	int seconds;
	int total_seconds;

	GameTimer() : timer(0.0f), seconds(0) {}

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
	void drawBorder();
	void killPlayer();
	void movePlayer(float dt);
	void handleCollisions(float dt);
	void moveStars(const v2& target, float dt);
	void createStar(const v2& pos);
	void addStar(const v2& pos, int count);
	void reflectVelocity(ID id, const v3& normal, float dt);

	ID _bomb_id;

	bool _grabbing;
	GameContext* _context;
	ds::World* _world;
	ID _player;
	ID _playerRing;
	float _playerAngle;
	ID _cursor;
	WanderingCubes* _wanderingCubes;
	SpottingCubes* _spottingCubes;
	//Cubes* _balls;
	Bombs* _bombs;
	//Stars* _stars;
	EventBuffer _buffer;
	v2 _cursor_pos;
	bool _showSettings;
	bool _showDebug;
	v2 _dialog_pos;
	GameTimer _game_timer;
	//KilledBall _killedBalls[MAX_BALLS];
	bool _dying;
	float _dying_timer;
	int _viewport_id;
	int _basic_viewport;
	ds::Color _border_color;
	NumberDefinitions _number_definitions;
	Numbers* _clock;
	Numbers* _points;

};

