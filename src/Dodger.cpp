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
	_settings.screenWidth = 1200;
	_settings.screenHeight = 720;
	_settings.clearColor = ds::Color(0,0,0,255);	
	_settings.initializeEditor = true;
	_context = new GameContext;
	_context->settings = new GameSettings;
	_context->particles = particles;
}

Dodger::~Dodger() {
	delete _context->settings;
	delete _context;
}

// -------------------------------------------------------
// Load content and prepare game
// -------------------------------------------------------
bool Dodger::loadContent() {
	int texture = ds::renderer::loadTexture("TextureArray");
	assert(texture != -1);
	ds::BitmapFont* font = ds::assets::loadFont("xscale", texture);
	ds::sprites::initializeTextSystem(font);
	gui::initialize();
	initializeGUI(font);
	ds::sprites::initializeTextSystem(font);
	_context->hudDialog = gui.get("HUD");
	// prepare particle system
	ds::Descriptor desc;
	desc.shader = ds::shader::createParticleShader();
	assert(desc.shader != 0);
	desc.texture = 0;
	desc.blendState = ds::renderer::getDefaultBlendState();
	_context->particles->init(desc);
	ds::assets::loadParticleSystem("particlesystems", _context->particles);
	stateMachine->add(new MainGameState(_context));
	stateMachine->add(new GameOverState(&gui,_context));
	stateMachine->add(new HighscoreState(&gui, _context));
	stateMachine->add(new MainMenuState(&gui, _context));
	stateMachine->connect("GameOver", 1, "MainGame");
	stateMachine->connect("GameOver", 2, "MainMenuState");
	stateMachine->connect("MainGame", 1, "GameOver");
	stateMachine->connect("MainMenuState", 3, "MainGame");

	// TEST
	Category root("root");
	if (json::read_simplified_json("content\\resources\\test.json", &root)) {
		int id = 0;
		root.getInt("id", &id);
		LOG << "---> ID: " << id;
	}
	return true;
}

void Dodger::init() {
	// for testing
	stateMachine->activate("MainGame");
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
	if (dlgID == 4 && button == 1) {
		shutdown();
	}
}
