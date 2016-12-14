#include "Enemies.h"
#include <core\io\json.h>

// -------------------------------------------------------
// Enemies
// -------------------------------------------------------
Enemies::Enemies(GameContext* context) : _ctx(context) {
	_timeline.load();
	_behaviorMap[SID("Follower")] = new FollowerBehavior(_ctx->world, _ctx->settings);
	_behaviorMap[SID("Wanderer")] = new WandererBehavior(_ctx->world, _ctx->settings);
	_behaviorMap[SID("Spotter")] = new SpotterBehavior(_ctx->world, _ctx->settings);
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
			e.behavior = _behaviorMap[it->type];
			e.id = id;
			e.energy = 2;
			e.behavior->create(id, target);
			_enemies.push_back(e);
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
	
	handleEvents(target);
}

// -------------------------------------------------------
// handle events
// -------------------------------------------------------
void Enemies::handleEvents(ID target) {
	if (_ctx->world->hasEvents()) {
		uint32_t n = _ctx->world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _ctx->world->getEvent(i);
			if (_ctx->world->contains(event.id)) {
				int type = _ctx->world->getType(event.id);
				int idx = findEnemy(event.id);
				if (idx != -1) {
					_enemies[idx].behavior->tick(event, target, 4);
				}
			}
		}
	}
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
