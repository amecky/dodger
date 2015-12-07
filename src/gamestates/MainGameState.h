#pragma once
#include <base\GameState.h>
#include <renderer\render_types.h>
#include <ui\IMGUI.h>
#include "..\GameContext.h"
#include <vector>
#include "..\EnergyBalls.h"
#include "..\Stars.h"
#include "..\Bombs.h"
#include "..\EventBuffer.h"
#include "..\Stars.h"

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
	ID _bomb_id;
	bool _grabbing;
	GameContext* _context;
	EnergyBalls* _balls;
	Bombs* _bombs;
	Stars* _stars;
	EventBuffer _buffer;
	v2 _cursor_pos;
	bool _showSettings;
	v2 _dialog_pos;
	GameTimer _game_timer;
	v2 _positions[MAX_BALLS];
	bool _dying;
	float _dying_timer;
	int _viewport_id;
	ds::Color _border_color;
};

