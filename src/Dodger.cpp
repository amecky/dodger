#include "Dodger.h"
#include "utils\Log.h"
#include "Constants.h"
#include <sprites\SpriteBatch.h>
#include <base\GameStateMachine.h>
#include "gamestates\MainGameState.h"
#include "gamestates\GameOverState.h"
#include "gamestates\MainMenuState.h"
#include "gamestates\HighscoreState.h"
#include "GameContext.h"
#include "GameSettings.h"

ds::BaseApp *app = new Dodger();

Dodger::Dodger() : ds::BaseApp() {
}

Dodger::~Dodger() {
	delete _context->settings;
	delete _context;
}

// -------------------------------------------------------
// Load content and prepare game
// -------------------------------------------------------
bool Dodger::loadContent() {
	_context = new GameContext;
	_context->settings = new GameSettings;
	_context->settings->load();
	_context->particles = particles;
	_context->hudDialog = gui->get("HUD");
	assert(_context->hudDialog != 0);
	addGameState(new MainGameState(_context));
	addGameState(new GameOverState(gui, _context));
	addGameState(new HighscoreState(gui, _context));
	addGameState(new MainMenuState(gui, _context));
	connectGameStates("GameOver", 1, "MainGame");
	connectGameStates("GameOver", 2, "MainMenuState");
	connectGameStates("MainGame", 1, "GameOver");
	connectGameStates("MainMenuState", 3, "MainGame");
	return true;
}

void Dodger::init() {
	// for testing
	activate("MainGame");
}


// -------------------------------------------------------
// Update
// -------------------------------------------------------
void Dodger::update(float dt) {
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void Dodger::draw() {
	
}

void Dodger::onGUIButton(ds::DialogID dlgID, int button) {
	LOG << "dialog: " << dlgID << " button:" << button;
	if (dlgID == 10 && button == 1) {
		shutdown();
	}
}
