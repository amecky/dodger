#include "AsteroidState.h"
#include <renderer\sprites.h>
#include "..\Constants.h"
#include <Vector.h>
#include <core\math\matrix.h>
#include <core\log\Log.h>
#include "base\InputSystem.h"
#include <core\math\GameMath.h>
#include <core\io\ReportWriter.h>

AsteroidState::AsteroidState(GameContext* context) : ds::GameState("AsteroidState"), _context(context) {
	
	_bullets = new Bullets(_context->world, context->settings);

	_hud = ds::res::getGUIDialog("HUD");
	_definitions.load();
}


AsteroidState::~AsteroidState() {
	delete _bullets;
}

// -------------------------------------------------------
// init
// -------------------------------------------------------
void AsteroidState::init() {
	
}

// -------------------------------------------------------
// split asteroid
// -------------------------------------------------------
void AsteroidState::splitAsteroid(ID id) {
	AsteroidData* data = (AsteroidData*)_context->world->get_data(id);
	const AsteroidInfo& info = _definitions.getDefinition(data->type);
	if (data->type < 3) {
		v3 r = _context->world->getRotation(id);
		v3 p = _context->world->getPosition(id);
		float angle = r.x + HALF_PI;
		v2 np = p.xy();
		math::addRadial(np, 60.0f, angle);
		startAsteroid(data->type + 1,np,angle);
		angle += PI;
		np = p.xy();
		math::addRadial(np, 60.0f, angle);
		startAsteroid(data->type + 1,np,angle);
	}
}

// -------------------------------------------------------
// start asteroid
// -------------------------------------------------------
void AsteroidState::startAsteroid(int type, const v2& pos, float angle) {	
	const AsteroidInfo& info = _definitions.getDefinition(type);
	ID id = _context->world->create(pos, info.objectHash);
	v2 vel = math::getRadialVelocity(angle, math::random(info.minVelocity,info.maxVelocity));
	_context->world->moveBy(id, vel);
	//_context->world->scaleAxes(id, 0, 1.0f, 0.8f, 1.5f, -1, tweening::easeSinus);
	//_context->world->scaleAxes(id, 1, 1.0f, 1.2f, 2.5f, -1, tweening::easeSinus);
	_context->world->attachCollider(id, ds::ShapeType::PST_CIRCLE, v2(info.radius));
	AsteroidData* data = (AsteroidData*)_context->world->attach_data(id, sizeof(AsteroidData), type);
	data->energy = info.energy;
	data->type = type;
}

// -------------------------------------------------------
// activate
// -------------------------------------------------------
void AsteroidState::activate() {
	_player = _context->world->create(v2(640, 360), math::buildTexture(40, 0, 40, 40), OT_PLAYER);
	_context->world->attachCollider(_player, ds::PST_CIRCLE, v2(40.f, 0.0));
	_playerAngle = 0.0f;
	_cursor = _context->world->create(v2(700, 384), math::buildTexture(40, 160, 20, 20), 100);
	_playerPrevious = v2(640, 360);
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
	_bullets->start(_player);
	return 0;
}

// -------------------------------------------------------
// kill player
// -------------------------------------------------------
void AsteroidState::killPlayer() {
	v2 wp = _context->world->getPosition(_player).xy();
	_bullets->killAll();
	_context->particles->start(1, wp);
	_context->world->remove(_player);
	_context->world->remove(_cursor);
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void AsteroidState::movePlayer(float dt) {
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
	_context->world->setPosition(_cursor, cp);
	v2 wp = _context->world->getPosition(_player).xy();
	v2 pos = wp;
	float angle = 0.0f;
	ds::math::followRelative(cp, wp, &_playerAngle, 5.0f, 1.1f * dt);
	_context->world->setRotation(_player, _playerAngle);
	pos += vel * 250.0f * dt;
	if (ds::math::isInside(pos, ds::Rect(0, 0, 1600, 900))) {			
		_context->world->setPosition(_player, pos);
		float distSqr = sqr_distance(pos, _playerPrevious);
		float dmin = 10.0f;
		if (distSqr > (dmin * dmin)) {
			_context->particles->start(10, _playerPrevious);
			_playerPrevious = pos;
		}
	}
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
int AsteroidState::update(float dt) {
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
	
	return 0;
}

// -------------------------------------------------------
// handle collisions
// -------------------------------------------------------
bool AsteroidState::handleCollisions(float dt) {
	ZoneTracker u2("MainGameState::handleCollisions");
	if (_context->world->hasCollisions()) {
		uint32_t n = _context->world->numCollisions();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::Collision& c = _context->world->getCollision(i);
			if (c.isBetween(OT_PLAYER, OT_BIG_ASTEROID)) {
				killEnemy(c, OT_BIG_ASTEROID);
				killPlayer();
				return true;
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
	if (_context->world->contains(id)) {
		AsteroidData* data = (AsteroidData*)_context->world->get_data(id);
		--data->energy;
		if (data->energy <= 0) {
			const AsteroidInfo& info = _definitions.getDefinition(data->type);
			v3 p = _context->world->getPosition(id);
			_context->particles->start(info.particlesID, p.xy());
			splitAsteroid(id);
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
	if (ascii == '1') {
		startAsteroid(0, v2(240, 160),math::random(0.0f,TWO_PI));
	}
	if (ascii == '2') {
		startAsteroid(1, v2(240, 160), math::random(0.0f, TWO_PI));
	}
	if (ascii == '3') {
		startAsteroid(2, v2(240, 160), math::random(0.0f, TWO_PI));
	}
	return 0;
}

