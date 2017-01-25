#include "Bullets.h"
#include <core\base\Assert.h>
#include <core\math\math.h>
#include "..\Constants.h"
#include <resources\ResourceContainer.h>
#include <particles\modules\PointEmitterModule.h>
#include <core\base\EventStream.h>
#include "SimplePlayer.h"
// ---------------------------------------
// Bullets
// ---------------------------------------
Bullets::Bullets(GameContext* context) : ds::GameObject("Bullets") , _context(context), _shooting(false) , _timer(0.0f) {
	_player = (SimplePlayer*)ds::game::get_game_object(SID("Player"));
	XASSERT(_player != 0, "We need to have a Player");
}

// ---------------------------------------------
// create
// ---------------------------------------------
void Bullets::create(float offset) {
	v3 wp = _context->world->getPosition(_player->getID());
	v3 r = _context->world->getRotation(_player->getID());
	v2 p = wp.xy();
	float oa = r.x + offset;
	math::addRadial(p, 30.0f, oa);
	ID bullet = _context->world->create(p, SID("Bullet"));
	_context->world->setRotation(bullet, r.x);
	_context->world->startBehavior(SID("bullets"), bullet);
	_context->world->attachCollider(bullet, ds::PST_CIRCLE, v2(18.0f));
}

// ---------------------------------------------
// tick
// ---------------------------------------------
void Bullets::tick(float dt) {
	if (_shooting) {
		ZoneTracker u1("Bullets:emitt");
		_timer += dt;
		if (_timer >= _context->settings->bullets.rate) {
			_timer -= _context->settings->bullets.rate;
			create(DEGTORAD(15.0f));
			create(-DEGTORAD(15.0f));
		}		
	}
	if (ds::events::containsType(ds::events::SE_BUTTON_ONE_DOWN)) {
		start();
	}
	if (ds::events::containsType(ds::events::SE_BUTTON_ONE_UP) && _shooting) {
		stop();
	}
}

// ---------------------------------------------
// kill all
// ---------------------------------------------
void Bullets::killAll() {
	_context->world->removeByType(OT_BULLET);
	_shooting = false;
}

// ---------------------------------------------
// start
// ---------------------------------------------
void Bullets::start() {
	if (!_shooting) {
		_shooting = true;
		_timer = 0.0f;
	}
}

// ---------------------------------------------
// stop
// ---------------------------------------------
void Bullets::stop() {
	_shooting = false;
	_timer = 0.0f;
}

// ---------------------------------------------
// kill
// ---------------------------------------------
void Bullets::kill(ID id) {
	if (_context->world->contains(id)) {
		v3 pos = _context->world->getPosition(id);
		_context->particles->start(8, pos.xy());
		_context->world->remove(id);
	}
}
