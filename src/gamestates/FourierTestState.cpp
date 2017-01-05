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

	_pathIndex = 0;
	_pathContainer.load();
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

	const FourierPath& path = _pathContainer.get(_pathIndex);
	Objects::iterator it = _objects.begin();
	while (it != _objects.end()) {
		it->timer += dt;

		float t = it->timer / path.ttl * TWO_PI;

		//float y = 4.0f * sin(_timer) / PI + 4.0f * sin(3.0f * _timer) / (3.0f * PI);
		//float y = 4.0f / PI * sin(it->timer) + 4.0f / (3.0f * PI) * sin(3.0f * it->timer) + 4.0f / (5.0f * PI) * sin(5.0f * it->timer);

		float v = 0.0f;
		for (int i = 0; i < path.num; ++i) {
			float f = static_cast<float>(i) * 2.0f + 1.0f;
			v += path.values[i] / PI * sin(f * t);
		}
		v3 p = _context->world->getPosition(it->id);
		v2 n = p.xy();
		n.y = it->y + v * path.height;// it->amplitude;
		n.x -= path.speed * dt;
		if (n.x < 10.0f) {
			_context->world->remove(it->id);
			it = _objects.remove(it);
		}
		else {
			//LOG << "p: " << p;
			float angle = math::getAngle(p.xy(),n);
			_context->world->setPosition(it->id, n);
			_context->world->setRotation(it->id, angle);
			++it;
		}
	}

	

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
				//killEnemy(c, OT_FOLLOWER);				
				//return true;
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
		Objects::iterator it = _objects.begin();
		while (it != _objects.end()) {
			if ( id == it->id) {
				_context->world->remove(it->id);
				it = _objects.remove(it);
			}
			else {
				++it;
			}
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
void FourierTestState::render() {
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	sprites->begin();
	v2 pos;
	FourierPath& path = _pathContainer.get(_pathIndex);
	for (int i = 0; i < 128; ++i) {
		pos.x = 1280 - i * 10;
		float t = static_cast<float>(i) / 128.0f * path.intervall * TWO_PI;
		float v = 0.0f;
		for (int i = 0; i < path.num; ++i) {
			if (path.values[i] > 0.0f) {
				float f = static_cast<float>(i) * 2.0f + 1.0f;
				v += path.values[i] / PI * sin(f * t);
				//v += path.values[i] * sin(f * t);
			}
		}
		pos.y = 360.0f + path.height * v;
		sprites->draw(pos, math::buildTexture(60, 120, 8, 8));
	}
	_hud->render();
	sprites->end();

	gui::start(1, &_dialogPos);
	gui::begin("Item", &_dialogState);
	gui::InputInt("Idx", &_pathIndex);
	gui::InputInt("Num", &path.num);
	gui::InputFloat("1.", &path.values[0]);
	gui::InputFloat("2.", &path.values[1]);
	gui::InputFloat("3.", &path.values[2]);
	gui::InputFloat("4.", &path.values[3]);
	gui::InputFloat("5.", &path.values[4]);
	gui::InputFloat("6.", &path.values[5]);
	gui::InputFloat("7.", &path.values[6]);
	gui::InputFloat("8.", &path.values[7]);
	gui::InputFloat("SPD", &path.speed);
	gui::InputFloat("Height", &path.height);
	gui::InputFloat("Intervall", &path.intervall);
	if (gui::Button("Start")) {
		FObject o;
		o.timer = 0.0f;// math::random(0.0f, TWO_PI);
		float oymin = 100.0f + abs(path.min) * path.height;
		float oymax = 620.0f - path.max * path.height;
		o.y = math::random(oymin, oymax);
		o.amplitude = math::random(70.0f, 110.0f);
		o.id = _context->world->create(v2(1280.0f, o.y), SID("Follower"));
		_context->world->attachCollider(o.id, ds::PST_CIRCLE);
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

