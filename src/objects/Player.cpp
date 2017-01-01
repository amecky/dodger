#include "Player.h"
#include <core\math\GameMath.h>
#include <base\InputSystem.h>

Player::Player(GameContext* ctx) : _context(ctx) , _dying(false) {
}


Player::~Player() {
}

void Player::activate() {
	_player = _context->world->create(v2(640, 360), math::buildTexture(40, 0, 40, 40), OT_PLAYER);
	_context->world->attachName(_player, "Player");
	_context->world->attachCollider(_player, ds::PST_CIRCLE, v2(40.f, 0.0));
	_playerAngle = 0.0f;
	_cursor = _context->world->create(v2(700, 384), math::buildTexture(40, 160, 20, 20), 100);
	_playerPrevious = v2(640, 360);
	_playerRing = _context->world->create(v2(640, 360), math::buildTexture(440, 0, 152, 152), OT_RING);
	_context->world->attachName(_playerRing, "Ring");
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void Player::move(float dt) {
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

void Player::kill() {
	v2 wp = _context->world->getPosition(_player).xy();
	//_bullets->killAll();
	_context->particles->start(1, wp);
	_context->world->remove(_player);
	_context->world->remove(_playerRing);
	_context->world->remove(_cursor);
	//_levelRunning = false;
}

