#include "Enemies.h"
#include <core\io\json.h>

// -------------------------------------------------------
// Enemies
// -------------------------------------------------------
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
// process queue
// -------------------------------------------------------
void Enemies::processQueue(float dt) {
	_timeline.tick(_queue, dt);
	while (!_queue.empty()) {
		const EmitterEvent& ed = _queue.top();
		_queue.pop();
		_ctx->particles->start(19, ed.pos);
		PendingEnemy pe;
		pe.timer = 0.0f;
		pe.pos = ed.pos;
		pe.type = ed.type;
		pe.behavior = ed.behavior;
		_pendings.push_back(pe);
	}
}

// -------------------------------------------------------
// process pending enemies
// -------------------------------------------------------
void Enemies::processPendingEnemies(ID target,float dt) {
	ds::Array<PendingEnemy>::iterator it = _pendings.begin();
	while (it != _pendings.end()) {
		it->timer += dt;
		if (it->timer >= 0.0f) {
			ID id = _ctx->world->create(it->pos, it->type);
			Enemy e;
			e.id = id;
			e.energy = 2;
			_enemies.push_back(e);
			_ctx->world->startBehavior(it->behavior, id);
			it = _pendings.remove(it);
		}
		else {
			++it;
		}
	}
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
void Enemies::tick(ID target, float dt) {
	
	processQueue(dt);

	processPendingEnemies(target, dt);
	
}

// -------------------------------------------------------
// find enemy
// -------------------------------------------------------
int Enemies::findEnemy(ID id) {
	for (uint32_t i = 0; i < _enemies.size(); ++i) {
		if (_enemies[i].id == id) {
			return i;
		}
	}
	return -1;
}

// -------------------------------------------------------
// kill
// -------------------------------------------------------
bool Enemies::kill(ID id) {
	int idx = findEnemy(id);
	if (idx != -1) {
		Enemy& e = _enemies[idx];
		--e.energy;
		if (e.energy <= 0) {
			return true;
		}
	}
	return false;
}
