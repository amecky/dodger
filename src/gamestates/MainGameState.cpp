#include "MainGameState.h"
#include <sprites\SpriteBatch.h>
#include "..\Constants.h"
#include <Vector.h>
#include <math\matrix.h>

MainGameState::MainGameState(GameContext* context) : ds::GameState("MainGame"), _context(context) {
	_balls = new EnergyBalls(_context);
	_bombs = new Bombs(_context);
	_stars = new Stars(_context);
	_showSettings = false;
	_dialog_pos = v2(740, 710);
	_grabbing = false;
	_dying = false;
	_dying_timer = 0.0f;
	_viewport_id = ds::renderer::createViewport(1280, 720, 1920, 1080);
	ds::renderer::setViewportPosition(_viewport_id, v2(960, 540));
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
	_border_color = ds::Color(192, 128, 0, 255);
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
	if (_bombs->grab(_context->world_pos, 75.0f, &_bomb_id)) {
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
	_context->particles->start(PLAYER_EXPLOSION, v3(_context->world_pos));
}
// -------------------------------------------------------
// Update
// -------------------------------------------------------
int MainGameState::update(float dt) {
	_context->debugPanel.reset();

	_context->debugPanel.show("World Pos", _context->world_pos);
	_context->debugPanel.show("Player Pos", _context->playerPosition);
	_context->debugPanel.show("Player angle", RADTODEG(_context->playerAngle));

	if (!_dying) {
		_cursor_pos = ds::renderer::getMousePosition();
		_context->debugPanel.show("Cursor", _cursor_pos);
		ds::mat4 wt = ds::matrix::mat4Transform(v2(-_context->world_pos.x, -_context->world_pos.y));
		ds::mat4 vw = ds::matrix::mat4Transform(v2(1920.0f / 2.0f, 1080.0f/ 2.0f));
		ds::mat4 inv = ds::matrix::mat4Inverse(wt * vw);
		//v2 cp = _cursor_pos * inv;
		v2 cp = _cursor_pos * 1.5f;

		ds::math::followRelative(cp, _context->playerPosition, &_context->playerAngle, 5.0f, 1.1f * dt);
		//ds::math::move_towards(_cursor_pos, _context->playerPosition, &_context->playerAngle, 80.0f, dt);

		_context->world_pos = _context->playerPosition;// *1.5f;
		ds::renderer::setViewportPosition(_viewport_id, _context->world_pos);

		if (_grabbing) {
			_bombs->follow(_bomb_id, _context->world_pos);
		}
		if (_game_timer.tick(dt)) {
			killPlayer();
		}
		_context->hudDialog->setNumber(8, _game_timer.seconds);

		_buffer.reset();

		//_balls->tick(dt);

		if (_balls->checkBallsInterception()) {
			LOG << "player hit ball";
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
					if (ds::math::checkCircleIntersection(_context->world_pos, PLAYER_RADIUS, event.position, 20.0f)) {
						LOG << "player within bomb explosion";
						killPlayer();
					}
				}
			}
		}

		_stars->tick(dt);
		int picked = _stars->pickup(_context->world_pos, PLAYER_RADIUS);
		if (picked > 0) {
			_game_timer.seconds += picked;
			if (_game_timer.seconds > 60) {
				_game_timer.seconds = 60;
			}
		}
		_stars->move(_context->world_pos, dt);

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
// draw border
// -------------------------------------------------------
void MainGameState::drawBorder() {
	ds::sprites::draw(v2(80, 1040), ds::math::buildTexture(840, 0, 40, 60), 0.0f, 1.0f, 1.0f, _border_color);
	ds::sprites::draw(v2(80, 40), ds::math::buildTexture(940, 0, 40, 60), 0.0f, 1.0f, 1.0f, _border_color);
	ds::sprites::draw(v2(1840, 1040), ds::math::buildTexture(840, 280, 40, 60), 0.0f, 1.0f, 1.0f, _border_color);
	ds::sprites::draw(v2(1840, 40), ds::math::buildTexture(940, 280, 40, 60), 0.0f, 1.0f, 1.0f, _border_color);
	for (int i = 0; i < 12; ++i) {
		ds::sprites::draw(v2(80, 110 + i * 80), ds::math::buildTexture(880, 0, 40, 80), 0.0f, 1.0f, 1.0f, _border_color);
		ds::sprites::draw(v2(1840, 110 + i * 80), ds::math::buildTexture(880, 280, 40, 80), 0.0f, 1.0f, 1.0f, _border_color);
	}
	for (int i = 0; i < 8; ++i) {
		ds::sprites::draw(v2(200 + i * 200, 1050), ds::math::buildTexture(840, 40, 200, 40), 0.0f, 1.0f, 1.0f, _border_color);
		ds::sprites::draw(v2(200 + i * 200, 30), ds::math::buildTexture(960, 40, 200, 40), 0.0f, 1.0f, 1.0f, _border_color);
	}
	ds::sprites::draw(v2(1760, 1050), ds::math::buildTexture(840, 40, 120, 40), 0.0f, 1.0f, 1.0f, _border_color);
	ds::sprites::draw(v2(1760, 30), ds::math::buildTexture(960, 40, 120, 40), 0.0f, 1.0f, 1.0f, _border_color);
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void MainGameState::render() {
	if (_showSettings) {
		_context->settings->showDialog(&_dialog_pos);
	}

	ds::renderer::selectViewport(_viewport_id);
	drawBorder();

	ds::sprites::draw(v2(960, 540), ds::math::buildTexture(840, 360, 120, 120));
	ds::sprites::draw(v2(60, 540), ds::math::buildTexture(840, 360, 120, 120));
	ds::sprites::draw(v2(60, 60), ds::math::buildTexture(840, 360, 120, 120));
	ds::sprites::draw(v2(60, 1020), ds::math::buildTexture(840, 360, 120, 120));
	ds::sprites::draw(v2(1860, 540), ds::math::buildTexture(840, 360, 120, 120));
	ds::sprites::draw(v2(1860, 60), ds::math::buildTexture(840, 360, 120, 120));
	ds::sprites::draw(v2(1860, 1020), ds::math::buildTexture(840, 360, 120, 120));
	/*
	_context->particles->render();
	_balls->render();
	if (_grabbing) {
		v2 bp = _bombs->getPosition(_bomb_id);
		v2 diff = _context->world_pos - bp;
		v2 n = normalize(diff);
		float angle = ds::vector::calculateRotation(n);
		angle += PI;
		ds::vector::addRadial(bp, 50.0f, angle);
		// FIXME: pulsate color
		ds::sprites::draw(bp, ds::math::buildTexture(40, 400, 40, 40),angle);
	}
	_bombs->render();
	_stars->render();

	
	*/
	if (!_dying) {
		ds::sprites::draw(_context->world_pos, ds::math::buildTexture(40, 0, 40, 42), _context->playerAngle);
		ds::sprites::draw(_context->world_pos, ds::math::buildTexture(440, 0, 152, 152));
	}

	ds::mat4 wt = ds::matrix::mat4Transform(v2(-_context->world_pos.x, -_context->world_pos.y));
	ds::mat4 vw = ds::matrix::mat4Transform(v2(1280.0f / 2.0f, 720.0f / 2.0f));
	ds::mat4 inv = ds::matrix::mat4Inverse(wt * vw);
	v2 cp = ds::renderer::getMousePosition() * inv;
	//v2 cp = ds::renderer::getMousePosition();
	//v2 cp = v2(mcp.x, mcp.y);
	_context->debugPanel.show("CM", cp);
	ds::sprites::draw(cp, ds::math::buildTexture(40, 160, 20, 20));

	ds::renderer::selectViewport(0);
	ds::sprites::draw(_cursor_pos, ds::math::buildTexture(40, 160, 20, 20));
	if (!_showSettings) {
		_context->debugPanel.render();
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

