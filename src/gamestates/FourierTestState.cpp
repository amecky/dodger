#include "FourierTestState.h"
#include <renderer\sprites.h>
#include "..\Constants.h"
#include <Vector.h>
#include <core\math\matrix.h>
#include <core\log\Log.h>
#include "base\InputSystem.h"
#include <core\math\GameMath.h>
#include <core\io\ReportWriter.h>
#include "..\objects\WarpingGrid.h"
#include "..\objects\ElasticBorder.h"

FourierTestState::FourierTestState(GameContext* context) : ds::GameState("FourierTestState"), _context(context) {
	
	_bullets = new Bullets(_context->world, context->settings);
	_hud = ds::res::getGUIDialog("HUD");
	_hud->setNumber(2, 0);
	_dialogState = 1;
	_dialogPos = v2(10, 710);

	for (int i = 0; i < 5; ++i) {
		_values[i] = 1.0f;
	}
	_num = 3;
	_xvel = 100.0f;
	_speed = 1.0f;
}


FourierTestState::~FourierTestState() {
	delete _bullets;
}

// -------------------------------------------------------
// init
// -------------------------------------------------------
void FourierTestState::init() {
	
}

// -------------------------------------------------------
// activate
// -------------------------------------------------------
void FourierTestState::activate() {
	_player = _context->world->create(v2(640, 360), math::buildTexture(40, 0, 40, 40), OT_PLAYER);
	_context->world->attachName(_player, "Player");
	_context->world->attachCollider(_player, ds::PST_CIRCLE, v2(40.f, 0.0));
	_playerAngle = 0.0f;
	_cursor = _context->world->create(v2(700, 384), math::buildTexture(40, 160, 20, 20), 100);
	_playerPrevious = v2(640, 360);
	_playerRing = _context->world->create(v2(640, 360), math::buildTexture(440, 0, 152, 152), OT_RING);
	_context->world->attachName(_playerRing, "Ring");
	_bullets->stop();	
	_hud->activate();

	_enemy = _context->world->create(v2(1200, 360), SID("Follower"));
	_timer = 0.0f;
}

// -------------------------------------------------------
// dactivate
// -------------------------------------------------------
void FourierTestState::deactivate() {
	killPlayer();
	_hud->deactivate();
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int FourierTestState::onButtonUp(int button, int x, int y) {
	_bullets->stop();
	return 0;
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int FourierTestState::onButtonDown(int button, int x, int y) {
	_bullets->start(_player);
	return 0;
}

void FourierTestState::killPlayer() {
	v2 wp = _context->world->getPosition(_player).xy();
	_bullets->killAll();
	_context->particles->start(1, wp);
	_context->world->remove(_player);
	_context->world->remove(_playerRing);
	_context->world->remove(_cursor);
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void FourierTestState::movePlayer(float dt) {
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
		_context->world->setPosition(_playerRing, pos);
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
int FourierTestState::update(float dt) {
	ZoneTracker u2("MainGameState::update");
	movePlayer(dt);

	const v2 wp = _context->world->getPosition(_player).xy();

	Objects::iterator it = _objects.begin();
	while (it != _objects.end()) {
		it->timer += dt * _speed;

		//float y = 4.0f * sin(_timer) / PI + 4.0f * sin(3.0f * _timer) / (3.0f * PI);
		float y = 4.0f / PI * sin(it->timer) + 4.0f / (3.0f * PI) * sin(3.0f * it->timer) + 4.0f / (5.0f * PI) * sin(5.0f * it->timer);

		float v = 0.0f;
		for (int i = 0; i < _num; ++i) {
			float f = static_cast<float>(i) * 2.0f + 1.0f;
			v += _values[i] / PI * sin(f * it->timer);
		}
		v3 p = _context->world->getPosition(it->id);
		p.y = it->y + v * 100.0f;
		p.x -= _xvel * dt;
		if (p.x < 10.0f) {
			_context->world->remove(it->id);
			it = _objects.remove(it);
		}
		else {
			//LOG << "p: " << p;
			_context->world->setPosition(it->id, p.xy());
			++it;
		}
	}
	/*
	_timer += dt * _speed;

	//float y = 4.0f * sin(_timer) / PI + 4.0f * sin(3.0f * _timer) / (3.0f * PI);
	float y = 4.0f / PI * sin(_timer) + 4.0f / (3.0f * PI) * sin( 3.0f * _timer) + 4.0f / (5.0f * PI) * sin(5.0f * _timer);

	float v = 0.0f;
	for (int i = 0; i < _num; ++i) {
		float f = static_cast<float>(i) * 2.0f + 1.0f;
		v += _values[i] / PI * sin(f * _timer);
	}
	v3 p = _context->world->getPosition(_enemy);
	p.y = 360 + v * 100.0f;
	p.x -= _xvel * dt;
	if (p.x < 40.0f) {
		p.x = 1200.0f;
	}
	//LOG << "p: " << p;
	_context->world->setPosition(_enemy, p.xy());
	*/
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
					v3 bp = _context->world->getPosition(event.id);
					_context->elasticBorder->splash(bp, -30);
					_bullets->kill(event.id);
				}
			}
		}
	}
	_bullets->tick(dt);

	_hud->tick(dt);
	/*
	if (_levelRunning && _levels->isActive() && (_kills == _levels->getNumberToKill())) {
		LOG << "ALL KILLED - NEXT LEVEL!!!";
		_levelRunning = false;
	}
	*/
	return 0;
}

// -------------------------------------------------------
// handle collisions
// -------------------------------------------------------
bool FourierTestState::handleCollisions(float dt) {
	ZoneTracker u2("MainGameState::handleCollisions");
	if (_context->world->hasCollisions()) {
		uint32_t n = _context->world->numCollisions();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::Collision& c = _context->world->getCollision(i);
			if (c.isBetween(OT_PLAYER, OT_FOLLOWER)) {
				killEnemy(c, OT_FOLLOWER);				
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
bool FourierTestState::killEnemy(const ds::Collision& c, int objectType) {
	bool ret = false;
	ID id = c.getIDByType(objectType);
	if (_context->world->contains(id)) {
	}
	if (c.containsType(OT_BULLET)) {
		_bullets->kill(c.getIDByType(OT_BULLET));
	}
	return ret;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void FourierTestState::render() {
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	sprites->begin();
	_hud->render();
	sprites->end();

	gui::start(1, &_dialogPos);
	gui::begin("Item", &_dialogState);
	gui::InputInt("Num", &_num);
	gui::InputFloat("1.", &_values[0]);
	gui::InputFloat("2.", &_values[1]);
	gui::InputFloat("3.", &_values[2]);
	gui::InputFloat("4.", &_values[3]);
	gui::InputFloat("5.", &_values[4]);
	gui::InputFloat("XV", &_xvel);
	gui::InputFloat("SPD", &_speed);
	if (gui::Button("Start")) {
		FObject o;
		o.timer = math::random(0.0f, TWO_PI);
		o.y = math::random(200.0f, 520.0f);
		o.id = _context->world->create(v2(1280.0f, o.y), SID("Follower"));
		_objects.push_back(o);
	}
	gui::end();
}
// -------------------------------------------------------
// on char
// -------------------------------------------------------
int FourierTestState::onChar(int ascii) {
	if (ascii == 'e') {
		return 1;
	}
	if (ascii == 'p') {
		return 2;
	}
	if (ascii == 'c') {
		for (int i = 0; i < 5; ++i) {
			float x = math::random(100.0f, 900.0f);
			float y = math::random(100.0f, 620.0f);
			//_context->particles->start(1, v2(x, y));
			_context->particles->start(5, v2(x, y));
			_context->particles->start(6, v2(x, y));
		}
	}
	return 0;
}

