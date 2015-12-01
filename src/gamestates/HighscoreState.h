#pragma once
#include "..\Constants.h"
#include "..\GameContext.h"
#include <base\GameState.h>

class HighscoreState : public ds::GameState {

public:
	HighscoreState(ds::DialogManager* gui, GameContext* context);
	~HighscoreState();
	int update(float dt);
	void render();
	int onGUIButton(ds::DialogID dlgID, int button);
	void activate();
	void deactivate();
private:
	GameContext* _context;
	ds::DialogManager* _gui;
};

