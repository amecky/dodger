#include "Stages.h"
#include <core\math\GameMath.h>
#include <base\InputSystem.h>
#include "WarpingGrid.h"

Stages::Stages(GameContext* ctx) : ds::GameObject("Stages"), _context(ctx) {
	_pathContainer.load();
	_state = StageState::IDLE;
	_emitted = 0;
	_currentPath = 0;
}


Stages::~Stages() {
}

// ------------------------------------------------------
// activate player - create new entity
// ------------------------------------------------------
void Stages::onActivation() {
	_state = StageState::PREPARING;
	_emitted = 0;
	_timer = 0.0f;
	_currentPath = 0;
}

// ------------------------------------------------------
// emitt enemy
// ------------------------------------------------------
void Stages::emittEnemy(float ypos) {
	StageObject o;
	o.timer = 0.0f;
	o.y = ypos;
	o.amplitude = math::random(70.0f, 110.0f);
	o.id = _context->world->create(v2(1280.0f, o.y), SID("Follower"));
	o.path = _currentPath;
	_context->world->attachCollider(o.id, ds::PST_CIRCLE);
	_objects.push_back(o);
}

// -------------------------------------------------------
// wait and move to next state
// -------------------------------------------------------
void Stages::wait(float dt, float ttl, StageState next) {
	_timer += dt;
	if (_timer >= ttl) {
		_timer = 0.0f;
		_state = next;
	}
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void Stages::tick(float dt) {
	ZoneTracker u2("Stages::tick");
	if (_state == StageState::PREPARING) {
		wait(dt, 2.0f, StageState::RUNNING);
	}
	else if (_state == StageState::WAITING) {
		wait(dt, 2.0f, StageState::RUNNING);
	}
	else if (_state == StageState::RUNNING) {
		_timer += dt;
		if (_timer > 0.3f) {
			emittEnemy(384.0f);
			_timer = 0.0f;
			++_emitted;
			if (_emitted >= 10) {
				_state = StageState::WAITING;
				_emitted = 0;
				++_currentPath;
				if (_currentPath > 5) {
					_currentPath = 0;
				}
			}
		}
	}
	moveObjects(dt);
}

// ------------------------------------------------------
// move objects
// ------------------------------------------------------
void Stages::moveObjects(float dt) {
	StageObjects::iterator it = _objects.begin();
	while (it != _objects.end()) {
		const ds::Path* path = _pathContainer.get(it->path);
		it->timer += dt;
		float t = it->timer / _pathContainer.getTTL(it->path);
		v2 pp;
		path->approx(t, &pp);
		v3 p = _context->world->getPosition(it->id);
		pp.y = it->y + pp.y;
		if (pp.x < 10.0f) {
			_context->world->remove(it->id);
			it = _objects.remove(it);
		}
		else {
			float angle = math::getAngle(p.xy(), pp);
			_context->world->setPosition(it->id, pp);
			_context->world->setRotation(it->id, angle);
			++it;
		}
	}
}

// ------------------------------------------------------
// 
// ------------------------------------------------------
void Stages::onDeactivation() {
}

// -------------------------------------------------------
// kill enemy
// -------------------------------------------------------
bool Stages::killEnemy(const ds::Collision& c, int objectType) {
	bool ret = false;
	ID id = c.getIDByType(objectType);
	if (_context->world->contains(id)) {
		StageObjects::iterator it = _objects.begin();
		while (it != _objects.end()) {
			if (id == it->id) {
				v3 p = _context->world->getPosition(id);
				_context->grid->applyForce(p.xy(), 0.3f, 5.0f, 40.0f);
				_context->world->remove(it->id);
				it = _objects.remove(it);
			}
			else {
				++it;
			}
		}
	}
	return ret;
}

