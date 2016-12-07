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
		LOG << "starting at " << ed.pos << " type: " << ed.type;
		// show starting spot
		_ctx->particles->start(19, ed.pos);

	}
}
