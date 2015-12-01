#include "MainGameState.h"
#include <sprites\SpriteBatch.h>
#include "..\Constants.h"
#include <Vector.h>

MainGameState::MainGameState(GameContext* context) : ds::GameState("MainGame"), _context(context) {
	_balls = new EnergyBalls(_context);
	_stars = new Stars(_context);
}


MainGameState::~MainGameState() {
	delete _stars;
	delete _balls;
}

// -------------------------------------------------------
// init
// -------------------------------------------------------
void MainGameState::init() {
}

// -------------------------------------------------------
// activate
// -------------------------------------------------------
void MainGameState::activate() {
	
	//_context->hudDialog->activate();

	//_context->hudDialog->setNumber(HUD_COLLECTED, 0);
	//_context->hudDialog->setNumber(HUD_BOMBS, 0);
	//_context->hudDialog->setNumber(HUD_LEVEL, 0);

	_context->playerPosition = v2(512, 384);

}

// -------------------------------------------------------
// dactivate
// -------------------------------------------------------
void MainGameState::deactivate() {
	_context->hudDialog->deactivate();
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int MainGameState::onButtonUp(int button, int x, int y) {
	return 0;
}
// -------------------------------------------------------
// Update
// -------------------------------------------------------
int MainGameState::update(float dt) {
	v2 mp = ds::renderer::getMousePosition();
	_context->playerPosition = mp;
	_balls->move(dt);
	_stars->tick(dt);
	_stars->pickup(mp, PLAYER_RADIUS);
	_stars->move(mp, dt);
	return 0;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void MainGameState::render() {
	_stars->render();
	_balls->render();
	ds::sprites::draw(_context->playerPosition, ds::math::buildTexture(50,0,40,48));
}

// -------------------------------------------------------
// on char
// -------------------------------------------------------
int MainGameState::onChar(int ascii) {	
	if (ascii == 'e') {
		return 1;
	}
	if (ascii == 's') {
		float x = ds::math::random(60.0f, 1140.0f);
		float y = ds::math::random(60.0f, 660.0f);
		_stars->add(v2(x, y));
	}
	return 0;
}

