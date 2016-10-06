#include "MainGameState.h"
#include <renderer\sprites.h>
#include "..\Constants.h"
#include <Vector.h>
#include <core\math\matrix.h>
#include <core\log\Log.h>
#include "base\InputSystem.h"
#include <core\math\GameMath.h>
#include <core\io\ReportWriter.h>

MainGameState::MainGameState(GameContext* context) : ds::GameState("MainGame"), _context(context) {
	_world = new ds::World;
	//_balls = new Cubes(_context);
	_bombs = new Bombs(_context, _world);
	//_stars = new Stars(_context);
	
	_player = _world->create(v2(512, 384), math::buildTexture(40, 0, 40, 40), OT_PLAYER);
	_world->attachCollider(_player, ds::PST_CIRCLE, v2(40.f, 0.0));
	_playerAngle = 0.0f;
	_cursor = _world->create(v2(700, 384), math::buildTexture(40, 160, 20, 20), 100);

	//_world->create(v2(200, 384), math::buildTexture(40, 0, 40, 42), 0.0f);
	_playerRing = _world->create(v2(100, 384), math::buildTexture(440, 0, 152, 152), OT_RING);

	_wanderingCubes = new WanderingCubes(_world);

	_showSettings = false;
	_showDebug = false;
	_dialog_pos = v2(10, 710);
	_grabbing = false;
	_dying = false;
	_dying_timer = 0.0f;
	//_viewport_id = ds::renderer::createViewport(1280, 720, 1600, 900);
	//_basic_viewport = ds::renderer::createViewport(1280, 720, 1280, 720);
	//ds::renderer::setViewportPosition(_viewport_id, v2(800, 450));

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
	delete _wanderingCubes;
	delete _world;
	//delete _stars;
	//delete _bombs;
	//delete _balls;
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
	/*
	//_context->hudDialog->activate();

	_context->points = 0;
	_context->hudDialog->setNumber(HUD_POINTS, 0);
	_context->hudDialog->setNumber(HUD_LEVEL, 1);
	_context->playerPosition = v2(640, 360);
	_context->playerAngle = 0.0f;

	_player_id = _world->create(_context->playerPosition, "Player");
	_ring_id = _world->create(_context->playerPosition, "Ring");
	_world->attachCollider(_player_id, OT_PLAYER, 0);

	_dying = false;
	_dying_timer = 0.0f;
	_balls->activate();
	_bombs->clear();
	_bombs->activate();
	_cursor_pos = v2(640, 360);

	_game_timer.reset(60);
	_context->hudDialog->setNumber(HUD_TIMER, _game_timer.seconds);
	// FIXME: remove all stars
	//_stars->clear();
	_grabbing = false;
	_border_color = ds::Color(192, 128, 0, 255);

	_clock->set(v2(640,640), 60, ds::Color(64,64,64,255));
	_points->set(v2(540, 60), 0, ds::Color(64, 64, 64, 255));
	*/
}

// -------------------------------------------------------
// dactivate
// -------------------------------------------------------
void MainGameState::deactivate() {
	//_context->hudDialog->deactivate();
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int MainGameState::onButtonUp(int button, int x, int y) {
	if (_grabbing) {
		_grabbing = false;
		//_bombs->burst(_bomb_id, 0.0f);
	}
	return 0;
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int MainGameState::onButtonDown(int button, int x, int y) {
	v3 pos = _world->getPosition(_player);
	if (_bombs->grab(pos.xy(), 75.0f, &_bomb_id)) {
		_grabbing = true;
	}
	return 0;
}

void MainGameState::killPlayer() {
	_dying = true;
	_dying_timer = 0.0f;
	//_balls->killAll();
	//_bombs->killAll();
	// FIXME: remove all stars
	//_stars->clear();
	//_context->particles->start(PLAYER_EXPLOSION, v3(_context->world_pos));
	_world->remove(_player);
	_world->remove(_playerRing);
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void MainGameState::movePlayer(float dt) {
	v2 cp = ds::input::getMousePosition();
	_world->setPosition(_cursor, cp);
	v2 wp = _world->getPosition(_player).xy();
	float angle = 0.0f;
	ds::math::followRelative(cp, wp, &_playerAngle, 5.0f, 1.1f * dt);
	_world->setRotation(_player, _playerAngle);
	if (ds::math::isInside(wp, ds::Rect(0, 0, 1024, 768))) {
		_world->setPosition(_player, wp);
		_world->setPosition(_playerRing, wp);
	}
}

// -------------------------------------------------------
// reflect velocity
// -------------------------------------------------------
void MainGameState::reflectVelocity(ID id,const v3& normal, float dt) {
	BombData* data = (BombData*)_world->get_data(id);
	float mdt = dot(data->velocity, normal);
	v3 ref = data->velocity - 2.0f * mdt * normal;
	_world->moveBy(id, ref);
	data->velocity = ref;
	v3 bp = _world->getPosition(id) + data->velocity * dt * 4.0f;
	_world->setPosition(id, bp.xy());
}
// -------------------------------------------------------
// Update
// -------------------------------------------------------
int MainGameState::update(float dt) {
	
	movePlayer(dt);

	const v2 wp = _world->getPosition(_player).xy();
	moveStars(wp, dt);

	_world->tick(dt);

	if (_grabbing) {
		_bombs->follow(_bomb_id, wp, dt);
	}

	uint32_t n = ds::events::num();
	if (n > 0) {
		for (uint32_t i = 0; i < n; ++i) {
			if (ds::events::getType(i) == 100) {
				LOG << "writing report now";
				ds::ReportWriter writer("reports\\world.html");
				_world->saveReport(writer);
			}
		}
	}

	_wanderingCubes->tick(dt);

	handleCollisions(dt);

	if (_world->hasEvents()) {
		uint32_t n = _world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _world->getEvent(i);
			if (event.action == ds::AT_BOUNCE) {
				v3* vel = (v3*)_world->getEventData(i);
				BombData* data = (BombData*)_world->get_data(event.id);
				data->velocity = *vel;
			}
		}
	}

	
	/*
	_context->debugPanel.reset();

	_context->debugPanel.show("World Pos", _context->world_pos);
	_context->debugPanel.show("Player Pos", _context->playerPosition);
	_context->debugPanel.show("Player angle", RADTODEG(_context->playerAngle));

	if (!_dying) {

		movePlayer(dt);

		_context->world->tick(dt);

		if (_grabbing) {
			_bombs->follow(_bomb_id, _context->world_pos);
		}
		if (_game_timer.tick(dt)) {
			killPlayer();
		}
		_clock->set(v2(640, 640), _game_timer.seconds, ds::Color(64, 64, 64, 255));
		_context->hudDialog->setNumber(HUD_TIMER, _game_timer.seconds);

		_buffer.reset();

		//_balls->tick(dt);
		_balls->move(dt);
		
		_buffer.reset();
		_bombs->tick(&_buffer, dt);

		// handle events
		const ds::ActionEventBuffer& actionBuffer = _context->world->getEventBuffer();
		_bombs->handleEvents(actionBuffer);
		_balls->handleEvents(actionBuffer);

		// handle game events
		if (_buffer.num > 0) {
			for (int i = 0; i < _buffer.num; ++i) {
				const GameEvent& event = _buffer.events[i];
				if (event.type == GameEvent::GE_BOMB_EXPLODED) {
					int killed = _balls->killBalls(event.position, _killedBalls);
					for (int j = 0; j < killed; ++j) {						
						int cnt = 0;
						switch (_killedBalls[j].type) {
							case 0: cnt = 1; _context->particles->start(BALL_EXPLOSION, v3(_killedBalls[j].position)); break;
							case 1: cnt = 2; _context->particles->start(BIG_CUBE_EXPLOSION, v3(_killedBalls[j].position)); break;
							case 2: cnt = 4; _context->particles->start(HUGE_CUBE_EXPLOSION, v3(_killedBalls[j].position)); break;
							default: cnt = 1; break;
						}
						addStar(_killedBalls[j].position,cnt);
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

		handleCollisions();

		moveStars(_context->world_pos, dt);

	}
	else {
		_dying_timer += dt;
		// FIXME: take value from config
		if (_dying_timer > 2.0f) {
			return 1;
		}
	}

	_context->particles->update(dt);
	*/
	return 0;
}

// -------------------------------------------------------
// handle collisions
// -------------------------------------------------------
void MainGameState::handleCollisions(float dt) {
	if (_world->hasCollisions()) {
		uint32_t n = _world->numCollisions();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::Collision& c = _world->getCollision(i);
			if (c.isBetween(OT_BOMB, OT_BOMB)) {
				v3 fp = _world->getPosition(c.firstID);
				v3 sp = _world->getPosition(c.secondID);
				v3 d = sp - fp;
				float l = length(d);
				v3 nd = normalize(d);

				reflectVelocity(c.firstID, nd, dt);
				reflectVelocity(c.secondID, nd, dt);
			}
			else if (c.isBetween(OT_PLAYER, OT_BOMB)) {
				ID bid = c.getIDByType(OT_BOMB);
				v3 fp = _world->getPosition(c.firstID);
				v3 sp = _world->getPosition(c.secondID);
				v3 d = sp - fp;
				v3 nd = normalize(d);
				reflectVelocity(bid, nd, dt);
			}
			else if (c.isBetween(OT_PLAYER, OT_STAR)) {
				ID id = c.getIDByType(OT_STAR);
				_world->remove(id);
			}
		}
	}
	// handle collisions
	/*
	int numCollisions = _world->getNumCollisions();
	int picked = 0;
	if (numCollisions > 0) {
		for (int i = 0; i < numCollisions; ++i) {
			const ds::Collision& collision = _world->getCollision(i);
			if (collision.containsType(OT_PLAYER) && collision.containsType(OT_STAR)) {
				LOG << "picked up star";
				ds::SID sid = collision.getSIDByType(OT_STAR);
				_world->remove(sid);
				++picked;
			}
			else if (collision.containsType(OT_PLAYER) && collision.containsType(OT_FOLLOWER)) {
				killPlayer();
			}
			else if (collision.containsType(OT_PLAYER) && collision.containsType(OT_HUGE_CUBE)) {
				killPlayer();
			}
			else if (collision.containsType(OT_PLAYER) && collision.containsType(OT_BIG_CUBE)) {
				killPlayer();
			}
		}
	}

	if (picked > 0) {
		_game_timer.seconds += picked;
		if (_game_timer.seconds > 60) {
			_game_timer.seconds = 60;
		}
	}
	*/
}

// -------------------------------------------------------
// draw border
// -------------------------------------------------------
void MainGameState::drawBorder() {
	/*
	// background
	ds::sprites::draw(v2(480, 306), ds::math::buildTexture(0, 512, 480, 306), 0.0f, 2.0f, 2.0f);
	ds::sprites::draw(v2(1280, 306), ds::math::buildTexture(0, 512, 320, 306), 0.0f, 2.0f, 2.0f);
	ds::sprites::draw(v2(480, 756), ds::math::buildTexture(0, 512, 480, 144), 0.0f, 2.0f, 2.0f);
	ds::sprites::draw(v2(1280, 756), ds::math::buildTexture(0, 512, 320, 144), 0.0f, 2.0f, 2.0f);
	// 4 corners
	ds::sprites::draw(v2(40, 860), ds::math::buildTexture(840, 0, 40, 60), 0.0f, 1.0f, 1.0f, _border_color);
	ds::sprites::draw(v2(40, 40), ds::math::buildTexture(940, 0, 40, 60), 0.0f, 1.0f, 1.0f, _border_color);
	ds::sprites::draw(v2(1560, 860), ds::math::buildTexture(840, 280, 40, 60), 0.0f, 1.0f, 1.0f, _border_color);
	ds::sprites::draw(v2(1560, 40), ds::math::buildTexture(940, 280, 40, 60), 0.0f, 1.0f, 1.0f, _border_color);
	// left and right wall
	for (int i = 0; i < 9; ++i) {
		ds::sprites::draw(v2(40, 110 + i * 80), ds::math::buildTexture(880, 0, 40, 80), 0.0f, 1.0f, 1.0f, _border_color);
		ds::sprites::draw(v2(1560, 110 + i * 80), ds::math::buildTexture(880, 280, 40, 80), 0.0f, 1.0f, 1.0f, _border_color);
	}
	// bottom and top wall
	for (int i = 0; i < 7; ++i) {
		ds::sprites::draw(v2(160 + i * 200, 870), ds::math::buildTexture(840, 40, 200, 40), 0.0f, 1.0f, 1.0f, _border_color);
		ds::sprites::draw(v2(160 + i * 200, 30), ds::math::buildTexture(960, 40, 200, 40), 0.0f, 1.0f, 1.0f, _border_color);
	}
	// missing left and right pieces
	ds::sprites::draw(v2(40, 810), ds::math::buildTexture(880, 0, 40, 40), 0.0f, 1.0f, 1.0f, _border_color);
	ds::sprites::draw(v2(1560, 810), ds::math::buildTexture(880, 280, 40, 40), 0.0f, 1.0f, 1.0f, _border_color);
	// missing top and bottom pieces
	ds::sprites::draw(v2(1505, 870), ds::math::buildTexture(840, 40, 90, 40), 0.0f, 1.0f, 1.0f, _border_color);
	ds::sprites::draw(v2(1505, 30), ds::math::buildTexture(960, 40, 90, 40), 0.0f, 1.0f, 1.0f, _border_color);
	*/
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void MainGameState::render() {	
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	ds::ChannelArray* array = _world->getChannelArray();
	v3* positions = (v3*)array->get_ptr(ds::WEC_POSITION);
	ds::Texture* textures = (ds::Texture*)array->get_ptr(ds::WEC_TEXTURE);
	v3* rotations = (v3*)array->get_ptr(ds::WEC_ROTATION);
	v3* scales = (v3*)array->get_ptr(ds::WEC_SCALE);
	ds::Color* colors = (ds::Color*)array->get_ptr(ds::WEC_COLOR);
	for (uint32_t i = 0; i < array->size; ++i) {
		v2 p = positions[i].xy();
		float r = rotations[i].x;
		ds::Texture t = textures[i];
		sprites->draw(positions[i].xy(), textures[i], rotations[i].x,scales[i].xy(),colors[i]);
	}
	/*
	ds::renderer::selectViewport(_viewport_id);
	drawBorder();
	ds::renderer::selectViewport(_basic_viewport);
	//if (_showSettings) {
		//_context->settings->showCompleteDialog(&_dialog_pos);
	//}
	if (_showDebug) {
		_context->debugPanel.showDialog(&_dialog_pos);
	}
	_clock->render();
	_points->render();
	ds::renderer::selectViewport(_viewport_id);
	_context->particles->render();
	//_balls->render();
	if (_grabbing) {
		v2 bp = _world->getPosition(_bomb_id);
		v2 diff = _context->world_pos - bp;
		v2 n = normalize(diff);
		float angle = ds::vector::calculateRotation(n);
		angle += PI;
		ds::vector::addRadial(bp, 50.0f, angle);
		// FIXME: pulsate color
		ds::sprites::draw(bp, ds::math::buildTexture(40, 400, 40, 40),angle);
	}
	_context->world->render();
	_bombs->render();
	
	if (!_dying) {
		//ds::sprites::draw(_context->world_pos, ds::math::buildTexture(40, 0, 40, 42), _context->playerAngle);
		//ds::sprites::draw(_context->world_pos, ds::math::buildTexture(440, 0, 152, 152));
	}


	ds::renderer::selectViewport(0);
	ds::sprites::draw(_cursor_pos, ds::math::buildTexture(40, 160, 20, 20));
	
	*/
}

// ---------------------------------------
// move towards player if in range
// ---------------------------------------
void MainGameState::moveStars(const v2& target, float dt) {
	ID ids[64];
	int num = _world->find_by_type(OT_STAR, ids, 64);
	for (int i = 0; i < num; ++i) {
		const v2 p = _world->getPosition(ids[i]).xy();
		v2 diff = target - p;
		if (sqr_length(diff) <  _context->settings->starMagnetRadius * _context->settings->starMagnetRadius) {
			v2 n = normalize(diff);
			n *= _context->settings->starSeekVelocity;
			v2 np = p + n * dt;
			_world->setPosition(ids[i], np);
		}
	}
}

// ---------------------------------------
// create star
// ---------------------------------------
void MainGameState::createStar(const v2& pos) {
	ID id = _world->create(pos, math::buildTexture(0, 40, 24, 24), OT_STAR, 0.0f, v2(1, 1), ds::Color(255, 180, 0, 255));
	_world->scaleByPath(id, &_context->settings->starScalePath, _context->settings->starFlashTTL);
	_world->attachCollider(id, ds::PST_CIRCLE, v2(24.0f, 24.0f));
	_world->removeAfter(id, _context->settings->starTTL);
}

// ---------------------------------------
// add new star
// ---------------------------------------
void MainGameState::addStar(const v2& pos, int count) {
	ID ids[64];
	int num = _world->find_by_type(OT_STAR, ids, 64);
	if (num + count < 128) {
		if (count == 1) {
			createStar(pos);
		}
		else {
			// spread out with radius = 20
			float step = TWO_PI / static_cast<float>(count);
			for (int i = 0; i < count; ++i) {
				v2 position;
				position.x = pos.x + 20.0f * cos(step * static_cast<float>(i));
				position.y = pos.y + 20.0f * sin(step * static_cast<float>(i));
				createStar(position);
			}
		}
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
	if (ascii == 'b') {
		_bombs->create();
	}
	if (ascii == 'x') {
		//_balls->killAll();
	}
	if (ascii == 'd') {
		_showDebug = !_showDebug;
	}
	if (ascii == '1') {
		_wanderingCubes->create();
	}
	if (ascii == '2') {
		//_balls->emitt(1);
	}
	if (ascii == '3') {
		//_balls->emitt(2);
	}
	if (ascii == '4') {
		addStar(v2(math::random(100,800), math::random(100,600)), 1);
	}
	if (ascii == '5') {
		addStar(v2(math::random(100, 800), math::random(100, 600)), 5);
	}
	//if (ascii == '6') {
		//_context->particles->startGroup(1, v3(512, 384, 0));
	//}
	return 0;
}

