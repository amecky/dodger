#pragma once
#include "..\Constants.h"
#include "..\GameContext.h"
#include <base\GameState.h>

class GameOverState : public ds::GameState {

public:
	GameOverState(ds::DialogManager* gui, GameContext* context);
	~GameOverState();
	int update(float dt);
	void render();
	int onGUIButton(ds::DialogID dlgID, int button);
	void activate();
	void deactivate();
private:
	GameContext* _context;
	ds::DialogManager* _gui;
};

