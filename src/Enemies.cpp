#include "Enemies.h"
#include <core\io\json.h>

Enemies::Enemies(GameContext* context) : _ctx(context) {
	_timeline.load();
}


Enemies::~Enemies() {
}

// -------------------------------------------------------
// start
// -------------------------------------------------------
void Enemies::start() {
	_timeline.start();
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
void Enemies::tick(float dt) {
	_timeline.tick(_queue, dt);
	while (!_queue.empty()) {
		const EmitterEvent& ed = _queue.top();
		_queue.pop();
		LOG << "starting at " << ed.pos;
		// show starting spot
		_ctx->particles->start(19, ed.pos);
		PendingEnemy pe;
		pe.timer = 0.0f;
		pe.pos = ed.pos;
		pe.type = ed.type;
		_pendings.push_back(pe);
	}
	ds::Array<PendingEnemy>::iterator it = _pendings.begin();
	while ( it != _pendings.end()) {
		it->timer += dt;
		if (it->timer >= 0.8f) {
			// create enemy
			LOG << "creating at " << it->pos;
			ID id = _ctx->world->create(it->pos, it->type);
			it = _pendings.remove(it);
		}
		else {
			++it;
		}
	}
}
