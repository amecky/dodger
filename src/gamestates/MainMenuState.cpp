#include "MainMenuState.h"
#include <core\log\Log.h>
#include <core\base\EventStream.h>
#include "..\asteroids\WarpingGrid.h"
#include <base\InputSystem.h>

const float SELECTION_MENU_TRANSITION = 0.4f;

MainMenuState::MainMenuState(GameContext* context) : ds::BasicMenuGameState("MainMenuState", "MainMenu"), _context(context) {
}


MainMenuState::~MainMenuState() {
}

void MainMenuState::activate() {
	_dialog->activate();
}

void MainMenuState::deactivate() {
	_dialog->deactivate();
}

int MainMenuState::update(float dt) {
	_dialog->tick(dt);
	v2 cp = ds::input::getMousePosition();
	_context->grid->applyForce(cp, 0.2f, 15.0f);	
	return 0;
}
