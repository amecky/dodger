#include "MainGameState.h"
#include <sprites\SpriteBatch.h>
#include "..\Constants.h"
#include <Vector.h>

MainGameState::MainGameState(GameContext* context) : ds::GameState("MainGame"), _context(context) {
	_balls = new EnergyBalls(_context);
	
}


MainGameState::~MainGameState() {
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
	_balls->activate();

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
	_balls->tick(dt);	
	_context->particles->update(dt);
	return 0;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void MainGameState::render() {
	ds::sprites::draw(v2(640, 360), ds::math::buildTexture(0, 512, 512, 384), 0.0f, 2.0f, 2.0f);
	_context->particles->render();
	_balls->render();
	ds::sprites::draw(_context->playerPosition, ds::math::buildTexture(40,0,42,42));
}

// -------------------------------------------------------
// on char
// -------------------------------------------------------
int MainGameState::onChar(int ascii) {	
	if (ascii == 'e') {
		return 1;
	}
	return 0;
}

