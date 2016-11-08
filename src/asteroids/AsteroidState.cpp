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
	_player.id = _context->world->create(v2(640, 360), math::buildTexture(40, 0, 40, 40), OT_PLAYER);
	_context->world->attachCollider(_player.id, ds::PST_CIRCLE, v2(40.f, 0.0));
	_player.angle = 0.0f;
	_cursor = _context->world->create(v2(700, 384), math::buildTexture(40, 160, 20, 20), 100);
	_player.previous = v2(640, 360);
	_bullets->stop();	
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
	v2 wp = _context->world->getPosition(_player.id).xy();
	_bullets->killAll();
	_context->particles->start(1, wp);
	_context->world->remove(_player.id);
	_context->world->remove(_cursor);
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void AsteroidState::movePlayer(float dt) {
	ZoneTracker u2("AsteroidState::movePlayer");
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
	if (ds::math::isInside(pos, ds::Rect(0, 0, 1600, 900))) {			
		_context->world->setPosition(_player.id, pos);
		float distSqr = sqr_distance(pos, _player.previous);
		float dmin = 10.0f;
		if (distSqr > (dmin * dmin)) {
			_context->particles->start(10, _player.previous);
			_player.previous = pos;
		}
	}
	//_context->grid->applyForce(pos, 0.2f, 20.0f);
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
int AsteroidState::update(float dt) {
	int idx = perf::startTimer("ASTEROID");
	ZoneTracker u2("AsteroidState::update");
	movePlayer(dt);

	const v2 wp = _context->world->getPosition(_player.id).xy();

	_context->world->tick(dt);
	perf::endTimer(idx);
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
// handle collisions
// -------------------------------------------------------
bool AsteroidState::handleCollisions(float dt) {
	ZoneTracker u2("AsteroidState::handleCollisions");
	if (_context->world->hasCollisions()) {
		uint32_t n = _context->world->numCollisions();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::Collision& c = _context->world->getCollision(i);
			//if (c.isBetween(OT_PLAYER, OT_BIG_ASTEROID)) {
				//killEnemy(c, OT_BIG_ASTEROID);
				//killPlayer();
				//return true;
			//}
			//else 
			if (c.isBetween(OT_BULLET, OT_BIG_ASTEROID)) {
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
	_hud->render();
	sprites->end();
}
// -------------------------------------------------------
// on char
// -------------------------------------------------------
int AsteroidState::onChar(int ascii) {
	if (ascii == 'e') {
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

