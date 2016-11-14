#include "AsteroidState.h"
#include <renderer\sprites.h>
#include "..\Constants.h"
#include <Vector.h>
#include <core\math\matrix.h>
#include <core\log\Log.h>
#include "base\InputSystem.h"
#include <core\math\GameMath.h>
#include <core\io\ReportWriter.h>
#include "WarpingGrid.h"

AsteroidState::AsteroidState(GameContext* context) : ds::GameState("AsteroidState"), _context(context) {
	
	_bullets = new Bullets(_context->world, context->settings);
	_asteroids = new Asteroids(context);
	_hud = ds::res::getGUIDialog("HUD");
	_stages.load();
	_activeStage = -1;
	_health = 100;
	_kills = 0;
	_points = 0;
	_player.id = INVALID_ID;
	_player.angle = 0.0f;
	_player.previous = v2(0, 0);

	_shapes.create(v2(640,360),  0);
	_shapes.create(v2(240, 360), 1);
	_shapes.create(v2(940, 360), 2);
	_shapes.create(v2(940, 360), 3);
}


AsteroidState::~AsteroidState() {
	delete _asteroids;
	delete _bullets;
}

// -------------------------------------------------------
// init
// -------------------------------------------------------
void AsteroidState::init() {
	
}

// -------------------------------------------------------
// activate
// -------------------------------------------------------
void AsteroidState::activate() {
	_health = 100;
	_kills = 0;
	_points = 0;
	_player.id = _context->world->create(v2(640, 360), math::buildTexture(40, 0, 40, 40), OT_PLAYER);
	_context->world->attachCollider(_player.id, ds::PST_CIRCLE, v2(40.f, 0.0));
	_player.angle = 0.0f;
	_cursor = _context->world->create(v2(700, 384), math::buildTexture(40, 160, 20, 20), 100);
	_player.previous = v2(640, 360);
	_bullets->stop();	
	_hud->setNumber(2, _health);
	_hud->setNumber(1, 0);
	_hud->activate();
}

// -------------------------------------------------------
// dactivate
// -------------------------------------------------------
void AsteroidState::deactivate() {
	_hud->deactivate();
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int AsteroidState::onButtonUp(int button, int x, int y) {
	_bullets->stop();
	return 0;
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int AsteroidState::onButtonDown(int button, int x, int y) {
	_bullets->start(_player.id);
	return 0;
}

// -------------------------------------------------------
// kill player
// -------------------------------------------------------
void AsteroidState::killPlayer() {
	ID pid = _player.id;
	v2 wp = _context->world->getPosition(pid).xy();
	_bullets->killAll();
	_context->particles->start(1, wp);
	_context->world->remove(pid);
	_context->world->remove(_cursor);
	_player.id = INVALID_ID;	
	int types[32];
	ID ids[256];
	int num = _stages.getActiveTypes(types, 32);
	for (int i = 0; i < num; ++i) {
		int n = _context->world->find_by_type(types[i],ids, 256);
		for (int j = 0; j < n; ++j) {
			_context->world->remove(ids[j]);
		}
	}
	_activeStage = -1;
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void AsteroidState::movePlayer(float dt) {
	ZoneTracker u2("AsteroidState::movePlayer");
	if (_player.id != INVALID_ID) {
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
		_context->world->setPosition(_cursor, cp);
		v2 wp = _context->world->getPosition(_player.id).xy();
		v2 pos = wp;
		float angle = 0.0f;
		ds::math::followRelative(cp, wp, &_player.angle, 5.0f, 1.1f * dt);
		_context->world->setRotation(_player.id, _player.angle);
		pos += vel * 250.0f * dt;
		if (ds::math::isInside(pos, ds::Rect(40, 40, 1200, 640))) {
			_context->world->setPosition(_player.id, pos);
			float distSqr = sqr_distance(pos, _player.previous);
			float dmin = 10.0f;
			if (distSqr > (dmin * dmin)) {
				_context->particles->start(10, _player.previous);
				_player.previous = pos;
			}
		}
	}
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
int AsteroidState::update(float dt) {
	ZoneTracker u2("AsteroidState::update");
	movePlayer(dt);
	_shapes.tick(dt);
	_context->world->tick(dt);
	{
		ZoneTracker u2("AsteroidState::events");
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
		ZoneTracker u2("AsteroidState::worldEvents");
		if (_context->world->hasEvents()) {
			uint32_t n = _context->world->numEvents();
			for (uint32_t i = 0; i < n; ++i) {
				const ds::ActionEvent& event = _context->world->getEvent(i);
				if (event.action == ds::AT_MOVE_BY && event.type == OT_BULLET) {
					_bullets->kill(event.id);
				}
				_asteroids->handleEvent(event);
			}
		}
	}
	_bullets->tick(dt);
	{
		ZoneTracker u2("AsteroidState::HUD");
		_hud->tick(dt);
	}
	{
		ZoneTracker u3("AsteroidState::stages");
		int types[16];
		int num = _stages.tick(dt, types, 16);
		if (num > 0) {
			for (int i = 0; i < num; ++i) {
				_asteroids->startAsteroid(types[i]);
			}
		}
	}
	if (_activeStage != -1 && _kills >= _stages.getTotalCount()) {
		LOG << "LEVEL DONE";
		_activeStage = -1;
		
	}

	return 0;
}

// -------------------------------------------------------
// handle player collisions
// -------------------------------------------------------
bool AsteroidState::handlePlayerCollision(const ds::Collision& c, int objectType) {
	if (c.isBetween(OT_PLAYER, objectType)) {
		if (!killEnemy(c, objectType)) {
			v3 pp = _context->world->getPosition(_player.id);
			ID aid = c.getIDByType(objectType);
			v3 ap = _context->world->getPosition(aid);
			v3 nd = normalize(ap - pp);
			_context->world->stopAction(aid, ds::ActionType::AT_MOVE_BY);
			_context->world->moveBy(aid, nd * 100.0f);
		}
		_health -= 10;
		if (_health <= 0) {
			killPlayer();
			// player died
			return true;
		}
		_hud->setNumber(2, _health);
	}
	return false;
}
// -------------------------------------------------------
// handle collisions
// -------------------------------------------------------
bool AsteroidState::handleCollisions(float dt) {
	ZoneTracker u2("AsteroidState::handleCollisions");
	if (_context->world->hasCollisions()) {
		uint32_t n = _context->world->numCollisions();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::Collision& c = _context->world->getCollision(i);
			if (c.isBetween(OT_PLAYER, OT_MEDIUM_ASTEROID)) {
				if (handlePlayerCollision(c, OT_MEDIUM_ASTEROID)) {
					return true;
				}				
			}
			else if (c.isBetween(OT_PLAYER, OT_BIG_ASTEROID)) {
				if (handlePlayerCollision(c, OT_BIG_ASTEROID)) {
					return true;
				}
			}
			if (c.isBetween(OT_PLAYER, OT_HUGE_ASTEROID)) {
				if (handlePlayerCollision(c, OT_HUGE_ASTEROID)) {
					return true;
				}
			}
			if (c.isBetween(OT_PLAYER, OT_SMALL_ASTEROID)) {
				if (handlePlayerCollision(c, OT_SMALL_ASTEROID)) {
					return true;
				}
			}
			else if (c.isBetween(OT_BULLET, OT_BIG_ASTEROID)) {
				killEnemy(c, OT_BIG_ASTEROID);
			}
			else if (c.isBetween(OT_BULLET, OT_HUGE_ASTEROID)) {
				killEnemy(c, OT_HUGE_ASTEROID);
			}
			else if (c.isBetween(OT_BULLET, OT_MEDIUM_ASTEROID)) {
				killEnemy(c, OT_MEDIUM_ASTEROID);
			}
			else if (c.isBetween(OT_BULLET, OT_SMALL_ASTEROID)) {
				killEnemy(c, OT_SMALL_ASTEROID);
			}
		}
	}
	return false;
}

// -------------------------------------------------------
// kill enemy
// -------------------------------------------------------
bool AsteroidState::killEnemy(const ds::Collision& c, int objectType) {
	bool ret = false;
	ID id = c.getIDByType(objectType);
	if (_asteroids->kill(id)) {
		// FIXME: calculate score correctly
		_points += 50;
		_hud->setNumber(1, _points);
		++_kills;		
		ret = true;
	}
	if (c.containsType(OT_BULLET)) {
		_bullets->kill(c.getIDByType(OT_BULLET));
	}
	return ret;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void AsteroidState::render() {
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	sprites->begin();	
	_shapes.render();
	_hud->render();
	sprites->end();
}
// -------------------------------------------------------
// on char
// -------------------------------------------------------
int AsteroidState::onChar(int ascii) {
	if (ascii == 'e') {
		killPlayer();
		return 1;
	}
	if (ascii == '5') {
		ds::ParticleSpawner* spawner = _context->particles->getParticleSystem(16)->getSpawner();
		spawner->rate = math::random(5, 64);
		_context->particles->start(18, v2(640, 360));
	}
	if (ascii == '6') {
		_context->grid->applyForce(p2i(2, 2), v2(2.5f, 2.5f));
	}
	if (ascii == 'q') {
		_kills = 0;
		_activeStage = 0;
		_stages.startStage(_activeStage);
	}
	if (ascii == '1') {
		_asteroids->startAsteroid(0);
	}
	if (ascii == '2') {
		_asteroids->startAsteroid(1);
	}
	if (ascii == '3') {
		_asteroids->startAsteroid(2);
	}
	if (ascii == '4') {
		_asteroids->startAsteroid(3);
	}
	return 0;
}

