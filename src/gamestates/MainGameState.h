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
private:
	GameContext* _context;
	EnergyBalls* _balls;
	Bombs* _bombs;
	EventBuffer _buffer;
	v2 _cursor_pos;
	
};

