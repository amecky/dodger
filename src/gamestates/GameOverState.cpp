#include "GameOverState.h"
#include <core\log\Log.h>
#include <core\base\EventStream.h>

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
