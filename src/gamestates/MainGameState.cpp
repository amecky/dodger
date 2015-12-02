#include "MainGameState.h"
#include <sprites\SpriteBatch.h>
#include "..\Constants.h"
#include <Vector.h>

MainGameState::MainGameState(GameContext* context) : ds::GameState("MainGame"), _context(context) {
	_balls = new EnergyBalls(_context);
	_bombs = new Bombs(_context);
	
}


MainGameState::~MainGameState() {
	delete _bombs;
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

	_context->playerPosition = v2(640, 360);
	_context->playerAngle = 0.0f;
	_balls->activate();
	_bombs->activate(2);
	_cursor_pos = v2(640, 360);
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
	_cursor_pos = ds::renderer::getMousePosition();
	float angle = 0.0f;
	ds::math::followRelative(_cursor_pos, _context->playerPosition, &_context->playerAngle, 5.0f, 5.0f * dt);
	//_context->playerPosition = mp;
	_buffer.reset();

	_balls->tick(dt);	

	_buffer.reset();
	_bombs->tick(&_buffer,dt);
	if (_buffer.num > 0) {
		for (int i = 0; i < _buffer.num; ++i) {
			const GameEvent& event = _buffer.events[i];
			if (event.type == GameEvent::GE_BOMB_EXPLODED) {
				_balls->killBalls(event.position);
			}
		}
	}
	
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
	_bombs->render();
	ds::sprites::draw(_cursor_pos, ds::math::buildTexture(40,160,20,20));
	ds::sprites::draw(_context->playerPosition, ds::math::buildTexture(40, 0, 40, 42),_context->playerAngle);
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

