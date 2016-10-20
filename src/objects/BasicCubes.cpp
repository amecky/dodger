#include "BasicCubes.h"
#include "resources\ResourceContainer.h"

v2 CircleCubeEmitter::get(int index, int total) {
	if (index == 0) {
		return _position;
	}
	++_cnt;
	if (_cnt > _threshold) {
		_threshold += 4;
		++_ring;
		_cnt = 0;
		_offset = math::random(0.0f, DEGTORAD(45.0f));
	}
	float step = TWO_PI / _threshold;
	float angle = step * static_cast<float>(index % _threshold) + math::random(DEGTORAD(-15.0f), DEGTORAD(15.0f));
	float r = _radius * _ring;
	return _position + math::getRadialVelocity(angle, _radius * _ring);
}

v2 LineCubeEmitter::get(int index, int total) {
	v2 p = _position;
	if (_vertical) {
		p.x += index * _distance;
	}
	else {
		p.y += index * _distance;
	}
	return p;
}

// -------------------------------------------------
// Behavior
// -------------------------------------------------

// -------------------------------------------------
// Spotter behavior
// -------------------------------------------------
void spotterBehavior(ds::World* world, const ds::ActionEvent& event, ID target) {
	if (event.action == ds::AT_SCALE_BY_PATH) {
		world->lookAt(event.id, target, 1.0f);
		world->attachCollider(event.id, ds::PST_CIRCLE, v2(48.0f, 48.0f));
	}
	else if (event.action == ds::AT_LOOK_AT) {
		v3 rot = world->getRotation(event.id);
		v3 v = math::getRadialVelocity(rot.x, 150.0f);
		float ttl = math::random(4.0f, 8.0f);
		world->moveBy(event.id, v, ttl);
	}
	else if (event.action == ds::AT_MOVE_BY) {
		world->lookAt(event.id, target, 1.0f);
	}
}

// -------------------------------------------------
// BasicCubes
// -------------------------------------------------
BasicCubes::BasicCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings) : _world(world), _emitter(emitter), _settings(settings), _timer(0.0f), _emitted(0), _running(false), _spawnDelay(0.0f) {
	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));

	_particles = ds::res::getParticleManager();
	_ready = false;
}

BasicCubes::~BasicCubes() {
}

// -------------------------------------------------
// reset
// -------------------------------------------------
void BasicCubes::reset() {
	_timer = 0.0f;
	_emitted = 0;
	_spawnDelay = 0.0f;
	_running = false;
}

// -------------------------------------------------
// init
// -------------------------------------------------
void BasicCubes::init(const SpawnSettings& spawnSettings) {
	_spawnSettings = spawnSettings;
	_spawnDelay = math::random(_spawnSettings.minDelay, _spawnSettings.maxDelay);
	_timer = 0.0f;
	_emitted = 0;
	_running = true;
}

// -------------------------------------------------
// tick
// -------------------------------------------------
void BasicCubes::tick(ID target, float dt) {
	if (_ready) {
		_timer += dt;
		if (_timer > 0.3f) {
			_ready = false;
			for (int i = 0; i < _num; ++i) {
				ID id = _world->create(_emitter->get(i, _num), _templateHash);
				float ttl = math::random(0.5f, 0.8f);
				_world->scaleByPath(id, &_scale_path, ttl);
				rotateTo(id, target);
				CubeData* data = (CubeData*)_world->attach_data(id, sizeof(CubeData), getObjectType());
				data->energy = _energy;
			}
		}
	}

	if (_emitted < _spawnSettings.maxCubes && _running) {
		_timer += dt;
		if (_timer > _spawnDelay) {
			create(target, _spawnSettings.emittCubes);
			++_emitted;
			_spawnDelay = math::random(_spawnSettings.minDelay, _spawnSettings.maxDelay);
		}
	}

	if (_world->hasEvents()) {
		uint32_t n = _world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _world->getEvent(i);
			if (_world->contains(event.id)) {
				int type = _world->getType(event.id);
				if (type == getObjectType()) {
					onEvent(event, target, dt);
				}
			}
		}
	}
}

// -------------------------------------------------
// rotateTo
// -------------------------------------------------
void BasicCubes::rotateTo(ID id, ID target) {
	v3 p = _world->getPosition(id);
	v3 t = _world->getPosition(target);
	float rotation = math::getAngle(p.xy(), t.xy());
	_world->setRotation(id, rotation);
}

// ---------------------------------------
// create
// ---------------------------------------
void BasicCubes::createCubes(StaticHash templateHash, ID target, int num, int energy) {
	_emitter->next();
	for (int i = 0; i < num; ++i) {
		v2 p = _emitter->get(i, num);
		_positions[i] = p;
		_particles->start(9, p);
	}
	_num = num;
	_energy = energy;
	_templateHash = templateHash;
	_timer = 0.0f;
	_ready = true;
}