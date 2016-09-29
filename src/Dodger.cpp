#include "Dodger.h"
#include <core\log\Log.h>
#include "Constants.h"
#include <renderer\sprites.h>
#include <gamestates\GameStateMachine.h>
#include "gamestates\MainGameState.h"
//#include "gamestates\GameOverState.h"
//#include "gamestates\MainMenuState.h"
//#include "gamestates\HighscoreState.h"
#include "GameContext.h"
#include "GameSettings.h"

ds::BaseApp *app = new Dodger();

Dodger::Dodger() : ds::BaseApp() {
}

Dodger::~Dodger() {
	//delete _context->world;
	delete _context->settings;
	delete _context;
}

// -------------------------------------------------------
// prepare
// -------------------------------------------------------
void Dodger::prepare(ds::Settings* settings) {
	settings->screenWidth = 1024;
	settings->screenHeight = 768;
	settings->clearColor = ds::Color(0, 0, 0, 255);
	settings->fullScreen = false;
	settings->reportingDirectory = "reports";
	settings->synched = true;
	settings->logTypes = ds::LogTypes::LT_CONSOLE | ds::LogTypes::LT_FILE;
}
// -------------------------------------------------------
// Load content and prepare game
// -------------------------------------------------------
bool Dodger::loadContent() {

	RID material = ds::res::find("SpriteMaterial", ds::ResourceType::MATERIAL);	
	ds::Material* m = ds::res::getMaterial(material);
	m->texture = ds::res::find("TextureArray", ds::ResourceType::TEXTURE);

	_context = new GameContext;
	_context->settings = new GameSettings;
	_context->settings->load();
	//_context->particles = particles;
	//_context->hudDialog = gui->get("HUD");
	//_context->world = new ds::World();
	ds::Rect r(50,50,1500,980);
	//_context->world->setBoundingRect(r);
	//assert(_context->hudDialog != 0);
	addGameState(new MainGameState(_context));
	//addGameState(new GameOverState(gui, _context));
	//addGameState(new HighscoreState(gui, _context));
	//addGameState(new MainMenuState(gui, _context));
	//connectGameStates("GameOver", 1, "MainGame");
	//connectGameStates("GameOver", 2, "MainMenuState");
	//connectGameStates("MainGame", 1, "GameOver");
	//connectGameStates("MainMenuState", 3, "MainGame");
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
void Dodger::render() {
	
}

