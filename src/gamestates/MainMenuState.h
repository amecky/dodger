#pragma once
#include "..\Constants.h"
#include "..\GameContext.h"
#include <base\GameState.h>

class MainMenuState : public ds::GameState {

public:
	MainMenuState(ds::DialogManager* gui, GameContext* context);
	~MainMenuState();
	int update(float dt);
	void render();
	int onGUIButton(ds::DialogID dlgID, int button);
	void activate();
	void deactivate();
private:
	GameContext* _context;
	ds::DialogManager* _gui;
};

