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

	_emitters.push_back(new TestCubeEmitter());
	_emitters.push_back(new RandomCubeEmitter());
	_emitters.push_back(new CircleCubeEmitter(40.0f));

	_wanderingCubes = new WanderingCubes(_world, _emitters[1], context->settings);
	_spottingCubes = new SpottingCubes(_world, _emitters[1], context->settings);
	_followerCubes = new FollowerCubes(_world, _emitters[2], context->settings);

	_world->ignoreCollisions(OT_FOLLOWER, OT_FOLLOWER);
	_world->ignoreCollisions(OT_PLAYER, OT_BULLET);
	_world->ignoreCollisions(OT_BULLET, OT_BULLET);

	_showSettings = false;
	_showDebug = false;
	_dialog_pos = v2(10, 710);
	_grabbing = false;
	_dying = false;
	_dying_timer = 0.0f;

	_testMode = false;
	_testTimer = 0.0f;
	//_viewport_id = ds::renderer::createViewport(1280, 720, 1600, 900);
	//_basic_viewport = ds::renderer::createViewport(1280, 720, 1280, 720);
	//ds::renderer::setViewportPosition(_viewport_id, v2(800, 450));


	_particles = ds::res::getParticleManager();

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
	
	_bulletTimer = 0.0f;
	_shooting = false;
}


MainGameState::~MainGameState() {
	delete _points;
	delete _clock;
	delete _wanderingCubes;
	delete _spottingCubes;
	delete _world;
	_emitters.destroy_all();
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
	_bulletTimer = 0.0f;
	_shooting = false;
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
	/*
	if (_grabbing) {
		_grabbing = false;
		//_bombs->burst(_bomb_id, 0.0f);
	}
	*/
	_shooting = false;
	return 0;
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int MainGameState::onButtonDown(int button, int x, int y) {
	/*
	v3 pos = _world->getPosition(_player);
	if (_bombs->grab(pos.xy(), 75.0f, &_bomb_id)) {
		_grabbing = true;
	}
	*/
	_shooting = true;
	_bulletTimer = 0.0f;
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
	v2 vel = v2(0.0f);
	if (ds::input::getKeyState('A')) {
		vel.x -= 1.0f;
	}
	if (ds::input::getKeyState('D')) {
		vel.x += 1.0f;
	}
	if (ds::input::getKeyState('W')) {
		vel.y += 1.0f;
	}
	if (ds::input::getKeyState('S')) {
		vel.y -= 1.0f;
	}
	v2 cp = ds::input::getMousePosition();	
	_world->setPosition(_cursor, cp);
	v2 wp = _world->getPosition(_player).xy();
	v2 pos = wp;
	float angle = 0.0f;
	ds::math::followRelative(cp, wp, &_playerAngle, 5.0f, 1.1f * dt);
	_world->setRotation(_player, _playerAngle);
	pos += vel * 250.0f * dt;
	if (ds::math::isInside(pos, ds::Rect(0, 0, 1024, 768))) {			
		_world->setPosition(_player, pos);
		_world->setPosition(_playerRing, pos);
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

	_wanderingCubes->tick(_player, dt);
	_spottingCubes->tick(_player, dt);
	_followerCubes->tick(_player, dt);
	
	handleCollisions(dt);

	if (_world->hasEvents()) {
		uint32_t n = _world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _world->getEvent(i);
			if (event.action == ds::AT_BOUNCE && event.type == OT_BOMB) {
				v3* vel = (v3*)_world->getEventData(i);
				BombData* data = (BombData*)_world->get_data(event.id);
				data->velocity = *vel;
			}
			else if (event.action == ds::AT_MOVE_BY && event.type == OT_BULLET) {
				_world->remove(event.id);
			}
		}
	}

	if (_shooting) {
		_bulletTimer += dt;
		if (_bulletTimer >= 0.1f) {
			_bulletTimer -= 0.1f;
			v3 wp = _world->getPosition(_player);
			v3 r = _world->getRotation(_player);
			v2 pos = wp.xy();
			math::addRadial(pos, 30.0f, r.x);
			ID bullet = _world->create(pos, math::buildTexture(0, 390, 18, 12),OT_BULLET,r.x,v2(1.0f),ds::Color(192,0,0,255));
			v2 vel = math::getRadialVelocity(r.x, 400.0f);
			_world->setRotation(bullet, r.x);
			_world->moveBy(bullet, vel, -1.0f, false);
			_world->attachCollider(bullet, ds::PST_CIRCLE, v2(18.0f));
		}
	}

	if (_testMode) {
		_testTimer += dt;
		if (_testTimer >= 5.0f) {
			_testTimer -= 5.0f;
			_followerCubes->create();
		}
	}
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
			/*
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
			else*/
			if (c.isBetween(OT_PLAYER, OT_STAR)) {
				ID id = c.getIDByType(OT_STAR);
				_world->remove(id);
			}
			else if (c.isBetween(OT_PLAYER, OT_FOLLOWER)) {
				ID id = c.getIDByType(OT_FOLLOWER);
				_world->remove(id);
			}
			else if (c.isBetween(OT_BULLET, OT_FOLLOWER)) {
				ID id = c.getIDByType(OT_FOLLOWER);
				if (_world->contains(id)) {
					v3 p = _world->getPosition(id);
					_particles->start(5, p.xy());
				}
				_world->remove(c.firstID);
				_world->remove(c.secondID);				
			}
		}
	}
	/*
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
	sprites->end();
	_particles->render();
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
	//if (ascii == 'd') {
		//_showDebug = !_showDebug;
	//}
	if (ascii == '1') {
		_wanderingCubes->create();
	}
	if (ascii == '2') {
		_spottingCubes->create();
	}
	if (ascii == '3') {
		_followerCubes->create();
	}
	if (ascii == '4') {
		addStar(v2(math::random(100,800), math::random(100,600)), 1);
	}
	if (ascii == '5') {
		addStar(v2(math::random(100, 800), math::random(100, 600)), 5);
	}
	if (ascii == '6') {
		for (int i = 0; i < 1; ++i) {
			float x = math::random(100.0f, 900.0f);
			float y = math::random(100.0f, 620.0f);
			_particles->start(5, v2(x,y));
		}
	}
	if (ascii == '7') {
		_testMode = !_testMode;
		if (_testMode) {
			_testTimer = 0.0f;
		}
	}
	return 0;
}

