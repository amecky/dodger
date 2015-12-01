#include "HighscoreState.h"
#include <utils\Log.h>
#include <sprites\SpriteBatch.h>
#include <renderer\graphics.h>
#include <base\GameStateMachine.h>

HighscoreState::HighscoreState(ds::DialogManager* gui, GameContext* context) : ds::GameState("HighscoreState"), _context(context), _gui(gui) {
}


HighscoreState::~HighscoreState() {
}

// --------------------------------------------
// activate
// --------------------------------------------
void HighscoreState::activate() {
	_gui->activate("Highscores");	
}

// --------------------------------------------
// activate
// --------------------------------------------
void HighscoreState::deactivate() {
	_gui->deactivate("Highscores");
}

// --------------------------------------------
// update
// --------------------------------------------
int HighscoreState::update(float dt) {
	return 0;
}

// --------------------------------------------
// click
// --------------------------------------------
int HighscoreState::onGUIButton(ds::DialogID dlgID, int button) {
	return button;
}

// --------------------------------------------
// render
// --------------------------------------------
void HighscoreState::render() {
	ds::sprites::draw(v2(512, 384), ds::math::buildTexture(0.0f, 512.0f, 512.0f, 384.0f), 0.0f, 2.0f, 2.0f);
}


