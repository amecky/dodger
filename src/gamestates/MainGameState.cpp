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
	
	_bullets = new Bullets(_context->world, context->settings);

	_levels = new Levels(_context->world, context->settings);
	_levels->load();

	_hud = ds::res::getGUIDialog("HUD");
	_levelRunning = false;
	_level = 1;
	_hud->setNumber(2, _level);
}


MainGameState::~MainGameState() {
	delete _levels;
	delete _bullets;
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
	_player = _context->world->create(v2(800, 450), math::buildTexture(40, 0, 40, 40), OT_PLAYER);
	_context->world->attachCollider(_player, ds::PST_CIRCLE, v2(40.f, 0.0));
	_playerAngle = 0.0f;
	_cursor = _context->world->create(v2(700, 384), math::buildTexture(40, 160, 20, 20), 100);

	_playerRing = _context->world->create(v2(100, 384), math::buildTexture(440, 0, 152, 152), OT_RING);
	_bullets->stop();	
	_hud->activate();
}

// -------------------------------------------------------
// dactivate
// -------------------------------------------------------
void MainGameState::deactivate() {
	_hud->deactivate();
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int MainGameState::onButtonUp(int button, int x, int y) {
	_bullets->stop();
	return 0;
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int MainGameState::onButtonDown(int button, int x, int y) {
	_bullets->start(_player);
	return 0;
}

void MainGameState::killPlayer() {
	v2 wp = _context->world->getPosition(_player).xy();
	_bullets->killAll();
	_levels->killAll();
	_context->particles->start(1, wp);
	_context->world->remove(_player);
	_context->world->remove(_playerRing);
	_context->world->remove(_cursor);
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void MainGameState::movePlayer(float dt) {
	ZoneTracker u2("MainGameState::movePlayer");
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
	const ds::Viewport& vp = graphics::getViewport(_context->viewport_id);
	v2 ncp = vp.convertToWorld(cp);
	_context->world->setPosition(_cursor, ncp);
	v2 wp = _context->world->getPosition(_player).xy();
	v2 pos = wp;
	float angle = 0.0f;
	ds::math::followRelative(ncp, wp, &_playerAngle, 5.0f, 1.1f * dt);
	_context->world->setRotation(_player, _playerAngle);
	pos += vel * 250.0f * dt;
	if (ds::math::isInside(pos, ds::Rect(0, 0, 1600, 900))) {			
		_context->world->setPosition(_player, pos);
		_context->world->setPosition(_playerRing, pos);
	}
	graphics::setViewportPosition(_context->viewport_id, pos);
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
int MainGameState::update(float dt) {
	ZoneTracker u2("MainGameState::update");
	movePlayer(dt);

	const v2 wp = _context->world->getPosition(_player).xy();

	_context->world->tick(dt);

	{
		ZoneTracker u2("MainGameState::events");
		uint32_t n = ds::events::num();
		if (n > 0) {
			for (uint32_t i = 0; i < n; ++i) {
				if (ds::events::getType(i) == 100) {
					LOG << "writing report now";
					ds::ReportWriter writer("reports\\world.html");
					_context->world->saveReport(writer);
				}
			}
		}
	}
	if (handleCollisions(dt)) {
		return 1;
	}
	{
		ZoneTracker u2("MainGameState::worldEvents");
		if (_context->world->hasEvents()) {
			uint32_t n = _context->world->numEvents();
			for (uint32_t i = 0; i < n; ++i) {
				const ds::ActionEvent& event = _context->world->getEvent(i);
				if (event.action == ds::AT_MOVE_BY && event.type == OT_BULLET) {
					_bullets->kill(event.id);
				}
			}
		}
	}
	_bullets->tick(dt);

	_hud->tick(dt);

	_levels->tick(_player, dt);

	if (_levelRunning && _levels->isActive() && (_kills == _levels->getNumberToKill())) {
		LOG << "ALL KILLED - NEXT LEVEL!!!";
		_levelRunning = false;
	}
	return 0;
}

// -------------------------------------------------------
// handle collisions
// -------------------------------------------------------
bool MainGameState::handleCollisions(float dt) {
	ZoneTracker u2("MainGameState::handleCollisions");
	if (_context->world->hasCollisions()) {
		uint32_t n = _context->world->numCollisions();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::Collision& c = _context->world->getCollision(i);
			if (c.isBetween(OT_PLAYER, OT_FOLLOWER)) {
				killEnemy(c, OT_FOLLOWER);
				killPlayer();
				return true;
			}
			else if (c.isBetween(OT_BULLET, OT_FOLLOWER)) {
				killEnemy(c, OT_FOLLOWER);				
			}
			else if (c.isBetween(OT_BULLET, OT_WANDERER)) {
				killEnemy(c, OT_WANDERER);
			}
			else if (c.isBetween(OT_BULLET, OT_SPOTTER)) {
				killEnemy(c, OT_SPOTTER);				
			}
		}
	}
	return false;
}

// -------------------------------------------------------
// kill enemy
// -------------------------------------------------------
bool MainGameState::killEnemy(const ds::Collision& c, int objectType) {
	bool ret = false;
	ID id = c.getIDByType(objectType);
	if (_context->world->contains(id)) {
		CubeData* data = (CubeData*)_context->world->get_data(id);
		--data->energy;
		if (data->energy <= 0) {
			v3 p = _context->world->getPosition(id);
			_context->particles->start(5, p.xy());
			_context->particles->start(6, p.xy());
			_context->world->remove(id);
			++_kills;
			ret = true;
		}
	}
	if (c.containsType(OT_BULLET)) {
		_bullets->kill(c.getIDByType(OT_BULLET));
	}
	return ret;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void MainGameState::render() {	
	graphics::selectViewport(_context->basic_viewport);
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	sprites->begin();
	_hud->render();
	sprites->end();
}
// -------------------------------------------------------
// on char
// -------------------------------------------------------
int MainGameState::onChar(int ascii) {	
	if (ascii == 'e') {
		return 1;
	}
	if (ascii == '+') {
		++_level;
		_hud->setNumber(2, _level);
	}
	if (ascii == '-') {
		--_level;
		if (_level < 0) {
			_level = 0;
		}
		_hud->setNumber(2, _level);
	}
	if (ascii == '2') {
		for (int i = 0; i < 5; ++i) {
			float x = math::random(100.0f, 900.0f);
			float y = math::random(100.0f, 620.0f);
			//_context->particles->start(1, v2(x, y));
			_context->particles->start(5, v2(x, y));
			_context->particles->start(6, v2(x, y));
		}
	}
	if (ascii == '1') {
		_levels->start(_level);
		_levelRunning = true;
		_kills = 0;
	}
	return 0;
}

