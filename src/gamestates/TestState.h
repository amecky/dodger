#pragma once
#include "..\Constants.h"
#include "..\GameContext.h"
#include <base\GameState.h>
#include <math\HexGrid.h>

struct GridItem {
	int index;
	v2 position;
};

class Player;

class TestState : public ds::GameState {

public:
	TestState(GameContext* context);
	~TestState();
	int update(float dt);
	void render();
	int onGUIButton(ds::DialogID dlgID, int button);
	void activate();
	void deactivate();
private:
	GameContext* _context;
	Player* _player;
	ds::HexGrid<GridItem> _grid;
};

