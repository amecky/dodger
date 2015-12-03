#include "MainGameState.h"
#include <sprites\SpriteBatch.h>
#include "..\Constants.h"
#include <Vector.h>

MainGameState::MainGameState(GameContext* context) : ds::GameState("MainGame"), _context(context) {
	_balls = new EnergyBalls(_context);
	_bombs = new Bombs(_context);
	_stars = new Stars(_context);
	_showSettings = true;
	_dialog_pos = v2(700, 710);
}


MainGameState::~MainGameState() {
	delete _stars;
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
	
	_context->hudDialog->activate();
	_context->points = 0;
	_context->hudDialog->setNumber(HUD_POINTS, 0);
	_context->hudDialog->setNumber(HUD_LEVEL, 1);
	//_context->hudDialog->setNumber(HUD_LEVEL, 0);

	_context->playerPosition = v2(640, 360);
	_context->playerAngle = 0.0f;
	_balls->activate();
	_bombs->activate(2);
	_cursor_pos = v2(640, 360);

	_game_timer.reset(60);
	_context->hudDialog->setNumber(8, _game_timer.seconds);
	_stars->clear();
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

	if (_game_timer.tick(dt)) {
		LOG << "GAME OVER";		
	}
	_context->hudDialog->setNumber(8, _game_timer.seconds);

	_buffer.reset();

	_balls->tick(dt);	

	_buffer.reset();
	_bombs->tick(&_buffer,dt);
	if (_buffer.num > 0) {
		for (int i = 0; i < _buffer.num; ++i) {
			const GameEvent& event = _buffer.events[i];
			if (event.type == GameEvent::GE_BOMB_EXPLODED) {
				int killed = _balls->killBalls(event.position,_positions);
				_context->points += killed;
				_context->hudDialog->setNumber(HUD_POINTS, _context->points);
				if (killed > 0) {
					for (int j = 0; j < killed; ++j) {
						_stars->add(_positions[j]);
					}
				}
			}
		}
	}
	
	_context->particles->update(dt);

	_stars->tick(dt);
	int picked = _stars->pickup(_context->playerPosition, PLAYER_RADIUS);
	if (picked > 0) {
		_game_timer.seconds += picked;
		if (_game_timer.seconds > 60) {
			_game_timer.seconds = 60;
		}
	}
	_stars->move(_context->playerPosition, dt);
	return 0;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void MainGameState::render() {
	ds::sprites::draw(v2(640, 360), ds::math::buildTexture(0, 512, 512, 384), 0.0f, 2.0f, 2.0f);
	if (_showSettings) {
		_context->settings->showDialog(&_dialog_pos);
	}
	_context->particles->render();
	_balls->render();
	_bombs->render();
	_stars->render();
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
	if (ascii == 'q') {
		_showSettings = !_showSettings;
	}
	return 0;
}

