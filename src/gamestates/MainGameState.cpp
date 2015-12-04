#include "MainGameState.h"
#include <sprites\SpriteBatch.h>
#include "..\Constants.h"
#include <Vector.h>

MainGameState::MainGameState(GameContext* context) : ds::GameState("MainGame"), _context(context) {
	_balls = new EnergyBalls(_context);
	_bombs = new Bombs(_context);
	_stars = new Stars(_context);
	_showSettings = false;
	_dialog_pos = v2(740, 710);
	_grabbing = false;
	_dying = false;
	_dying_timer = 0.0f;
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
	_dying = false;
	_dying_timer = 0.0f;
	_balls->activate();
	_bombs->clear();
	_bombs->activate(2);
	_cursor_pos = v2(640, 360);

	_game_timer.reset(60);
	_context->hudDialog->setNumber(8, _game_timer.seconds);
	_stars->clear();
	_grabbing = false;
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
	if (_grabbing) {
		_grabbing = false;
		_bombs->burst(_bomb_id, 0.0f);
	}
	return 0;
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int MainGameState::onButtonDown(int button, int x, int y) {
	if (_bombs->grab(_context->playerPosition, 75.0f,&_bomb_id)) {
		_grabbing = true;
	}
	return 0;
}

void MainGameState::killPlayer() {
	_dying = true;
	_dying_timer = 0.0f;
	_balls->killAll();
	_bombs->killAll();
	_stars->clear();
	_context->particles->start(PLAYER_EXPLOSION, v3(_context->playerPosition));
}
// -------------------------------------------------------
// Update
// -------------------------------------------------------
int MainGameState::update(float dt) {
	if (!_dying) {
		_cursor_pos = ds::renderer::getMousePosition();
		float angle = 0.0f;
		ds::math::followRelative(_cursor_pos, _context->playerPosition, &_context->playerAngle, 5.0f, 5.0f * dt);

		if (_grabbing) {
			_bombs->follow(_bomb_id, _context->playerPosition);
		}
		if (_game_timer.tick(dt)) {
			killPlayer();
		}
		_context->hudDialog->setNumber(8, _game_timer.seconds);

		_buffer.reset();

		_balls->tick(dt);

		if (_balls->checkBallsInterception()) {
			killPlayer();
		}

		_buffer.reset();
		_bombs->tick(&_buffer, dt);
		if (_buffer.num > 0) {
			for (int i = 0; i < _buffer.num; ++i) {
				const GameEvent& event = _buffer.events[i];
				if (event.type == GameEvent::GE_BOMB_EXPLODED) {
					int killed = _balls->killBalls(event.position, _positions);
					for (int j = 0; j < killed; ++j) {
						_context->particles->start(BOMB_EXPLOSION, v3(_positions[j]));
						_stars->add(_positions[j]);
					}
					_context->points += killed;
					_context->hudDialog->setNumber(HUD_POINTS, _context->points);
					if (ds::math::checkCircleIntersection(_context->playerPosition, PLAYER_RADIUS, event.position, 20.0f)) {
						killPlayer();
					}
				}
			}
		}

		_stars->tick(dt);
		int picked = _stars->pickup(_context->playerPosition, PLAYER_RADIUS);
		if (picked > 0) {
			_game_timer.seconds += picked;
			if (_game_timer.seconds > 60) {
				_game_timer.seconds = 60;
			}
		}
		_stars->move(_context->playerPosition, dt);

	}
	else {
		_dying_timer += dt;
		// FIXME: take value from config
		if (_dying_timer > 2.0f) {
			return 1;
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
	if (_showSettings) {
		_context->settings->showDialog(&_dialog_pos);
	}
	_context->particles->render();
	_balls->render();
	if (_grabbing) {
		v2 bp = _bombs->getPosition(_bomb_id);
		v2 diff = _context->playerPosition - bp;
		v2 n = normalize(diff);
		float angle = ds::vector::calculateRotation(n);
		angle += PI;
		ds::vector::addRadial(bp, 50.0f, angle);
		// FIXME: pulsate color
		ds::sprites::draw(bp, ds::math::buildTexture(40, 400, 40, 40),angle);
	}
	_bombs->render();
	_stars->render();
	ds::sprites::draw(_cursor_pos, ds::math::buildTexture(40,160,20,20));
	if (!_dying) {
		ds::sprites::draw(_context->playerPosition, ds::math::buildTexture(40, 0, 40, 42), _context->playerAngle);
		ds::sprites::draw(_context->playerPosition, ds::math::buildTexture(440, 0, 152, 152));
	}
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
	if (ascii == 'w') {
		_context->particles->start(5, v3(640,360,0));
	}
	return 0;
}

