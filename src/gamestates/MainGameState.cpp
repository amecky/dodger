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

	_number_definitions.define(0, ds::Rect(300,   0, 49, 33));
	_number_definitions.define(1, ds::Rect(300,  49, 21, 33));
	_number_definitions.define(2, ds::Rect(300,  70, 46, 33));
	_number_definitions.define(3, ds::Rect(300, 116, 46, 33));
	_number_definitions.define(4, ds::Rect(300, 162, 48, 33));
	_number_definitions.define(5, ds::Rect(300, 210, 45, 33));
	_number_definitions.define(6, ds::Rect(300, 255, 49, 33));
	_number_definitions.define(7, ds::Rect(300, 304, 48, 33));
	_number_definitions.define(8, ds::Rect(300, 352, 49, 33));
	_number_definitions.define(9, ds::Rect(300, 401, 49, 33));

	_clock = new Numbers(_number_definitions, 2);
	_points = new Numbers(_number_definitions, 6);
}


MainGameState::~MainGameState() {
	delete _points;
	delete _clock;
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
	
	//_context->hudDialog->activate();
	_context->points = 0;
	_context->hudDialog->setNumber(HUD_POINTS, 0);
	_context->hudDialog->setNumber(HUD_LEVEL, 1);
	_context->playerPosition = v2(640, 360);
	_context->playerAngle = 0.0f;
	_dying = false;
	_dying_timer = 0.0f;
	_balls->activate();
	_bombs->clear();
	_bombs->activate(4);
	_cursor_pos = v2(640, 360);

	_game_timer.reset(60);
	_context->hudDialog->setNumber(8, _game_timer.seconds);
	_stars->clear();
	_grabbing = false;
	_border_color = ds::Color(192, 128, 0, 255);

	_clock->set(v2(640,640), 60, ds::Color(64,64,64,255));
	_points->set(v2(540, 60), 0, ds::Color(64, 64, 64, 255));

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
// move player
// -------------------------------------------------------
void MainGameState::movePlayer(float dt) {
	_cursor_pos = ds::renderer::getMousePosition();
	_context->debugPanel.show("Cursor", _cursor_pos);
	v2 wp;
	float dx = _context->world_pos.x - 1280.0f / 2.0f;
	if (dx < 0.0f) {
		dx = 0.0f;
	}
	if (dx > 640.0f) {
		dx = 640.0f;
	}
	wp.x = _cursor_pos.x + dx;

	float dy = _context->world_pos.y - 720.0f / 2.0f;
	if (dy < 0.0f) {
		dy = 0.0f;
	}
	if (dy > 360.0f) {
		dy = 360.0f;
	}
	wp.y = _cursor_pos.y + dy;
	_context->debugPanel.show("WP", wp);

	ds::math::followRelative(wp, _context->playerPosition, &_context->playerAngle, 5.0f, 1.1f * dt);
	ds::vector::clamp(_context->playerPosition, v2(100, 50), v2(1820, 1030));
	_context->world_pos = _context->playerPosition;
	ds::renderer::setViewportPosition(_viewport_id, _context->world_pos);
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

		movePlayer(dt);

		if (_grabbing) {
			_bombs->follow(_bomb_id, _context->world_pos);
		}
		if (_game_timer.tick(dt)) {
			killPlayer();
		}
		_clock->set(v2(640, 640), _game_timer.seconds, ds::Color(64, 64, 64, 255));
		_context->hudDialog->setNumber(8, _game_timer.seconds);

		_buffer.reset();

		_balls->tick(dt);

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
					int killed = _balls->killBalls(event.position, _killedBalls);
					for (int j = 0; j < killed; ++j) {
						_context->particles->start(BOMB_EXPLOSION, v3(_killedBalls[j].position));
						int cnt = 0;
						switch (_killedBalls[j].type) {
							case EBT_FOLLOWER: cnt = 1; break;
							case EBT_BIG_CUBE: cnt = 2; break;
							case EBT_HUGE_CUBE: cnt = 4; break;
							default: cnt = 1; break;
						}
						_stars->add(_killedBalls[j].position,cnt);
					}
					_context->points += killed;
					_context->hudDialog->setNumber(HUD_POINTS, _context->points);
					_points->set(v2(540, 60), _context->points, ds::Color(32, 32, 32, 255));
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

	ds::sprites::draw(v2(480, 268), ds::math::buildTexture(0, 512, 480, 272),0.0f,2.0f,2.0f);
	ds::sprites::draw(v2(480, 812), ds::math::buildTexture(0, 512, 480, 272), 0.0f, 2.0f, 2.0f);
	ds::sprites::draw(v2(1440, 268), ds::math::buildTexture(0, 512, 480, 272), 0.0f, 2.0f, 2.0f);
	ds::sprites::draw(v2(1440, 812), ds::math::buildTexture(0, 512, 480, 272), 0.0f, 2.0f, 2.0f);

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
	ds::renderer::selectViewport(0);
	_clock->render();
	_points->render();
	ds::renderer::selectViewport(_viewport_id);
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
	
	if (!_dying) {
		ds::sprites::draw(_context->world_pos, ds::math::buildTexture(40, 0, 40, 42), _context->playerAngle);
		ds::sprites::draw(_context->world_pos, ds::math::buildTexture(440, 0, 152, 152));
	}


	ds::renderer::selectViewport(0);
	ds::sprites::draw(_cursor_pos, ds::math::buildTexture(40, 160, 20, 20));
	
	//if (!_showSettings) {
		//_context->debugPanel.render();
	//}
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
	if (ascii == '2') {
		_balls->emitt(EBT_BIG_CUBE, 1);
	}
	if (ascii == '3') {
		_balls->emitt(EBT_HUGE_CUBE, 1);
	}
	return 0;
}

