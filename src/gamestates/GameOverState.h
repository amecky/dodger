#pragma once
#include "..\GameContext.h"
#include <gamestates\GameState.h>

class GameOverState : public ds::BasicMenuGameState {

public:
	GameOverState(GameContext* context);
	~GameOverState();
	void activate();
	void deactivate();
private:
	GameContext* _context;
};

