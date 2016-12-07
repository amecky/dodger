#include "GameOverState.h"
#include <core\log\Log.h>
#include <core\base\EventStream.h>
#include "..\objects\WarpingGrid.h"
#include <base\InputSystem.h>

const float SELECTION_MENU_TRANSITION = 0.4f;

GameOverState::GameOverState(GameContext* context) : ds::BasicMenuGameState("GameOver", "GameOver"), _context(context) {
}


GameOverState::~GameOverState() {
}

void GameOverState::activate() {
	_dialog->activate();
}

void GameOverState::deactivate() {
	_dialog->deactivate();
}

int GameOverState::update(float dt) {
	_dialog->tick(dt);
	v2 cp = ds::input::getMousePosition();
	_context->grid->applyForce(cp, 0.2f, 15.0f);	
	return 0;
}
