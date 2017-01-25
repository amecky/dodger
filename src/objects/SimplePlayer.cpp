#include "SimplePlayer.h"
#include <core\math\GameMath.h>
#include <base\InputSystem.h>

SimplePlayer::SimplePlayer(GameContext* ctx) : ds::GameObject("Player") , _context(ctx) , _velocity(250.0f) {
}


SimplePlayer::~SimplePlayer() {
}

// ------------------------------------------------------
// activate player - create new entity
// ------------------------------------------------------
void SimplePlayer::onActivation() {
	_id = _context->world->create(v2(100, 384), math::buildTexture(40, 0, 40, 40), OT_PLAYER);
	_context->world->attachName(_id, "Player");
	_context->world->attachCollider(_id, ds::PST_CIRCLE, v2(40.f, 0.0));
	_previous = v2(100, 384);
	_velocity = 250.0f;
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void SimplePlayer::tick(float dt) {
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
	v2 wp = _context->world->getPosition(_id).xy();
	v2 pos = wp;
	pos += vel * _velocity * dt;
	if (ds::math::isInside(pos, ds::Rect(0, 0, 1024, 768))) {
		_context->world->setPosition(_id, pos);
		float distSqr = sqr_distance(pos, _previous);
		float dmin = 10.0f;
		if (distSqr > (dmin * dmin)) {
			_context->particles->start(10, _previous);
			_previous = pos;
		}
	}
}

// ------------------------------------------------------
// kill player
// ------------------------------------------------------
void SimplePlayer::onDeactivation() {
	v2 wp = _context->world->getPosition(_id).xy();
	_context->particles->start(1, wp);
	_context->world->remove(_id);
}


