#pragma once
#include "..\GameContext.h"
#include <gamestates\GameState.h>

class MainMenuState : public ds::BasicMenuGameState {

public:
	MainMenuState(GameContext* context);
	~MainMenuState();
	void activate();
	void deactivate();
	virtual int update(float dt);
private:
	GameContext* _context;
};

