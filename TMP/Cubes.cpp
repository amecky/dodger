#include "Cubes.h"
#include <core\math\Bitset.h>

// ---------------------------------------
// load cube definitions
// ---------------------------------------
bool CubeDefinitions::loadData(const ds::JSONReader& reader) {
	int cats[32];
	int num = reader.get_categories(cats, 32);
	for (int i = 0; i < num; ++i) {
		CubeDefinition def;
		reader.get_int(cats[i], "type", &def.type);
		sprintf_s(def.name, 20, "%s", reader.get_string(cats[i],"name"));
		reader.get_float(cats[i], "velocity", &def.velocity);
		reader.get_float(cats[i], "velocity_variance", &def.velocityVariance);
		reader.get_int(cats[i], "num_stars", &def.numStars);
		reader.get_int(cats[i], "behavior_bits", &def.behaviorBits);
		reader.get_float(cats[i], "grow_ttl", &def.growTTL);
		_definitions.push_back(def);
	}
	return true;
}

// ---------------------------------------
// load wave definitions
// ---------------------------------------
bool WaveDefinitions::loadData(const ds::JSONReader& reader) {
	int cats[32];
	int num = reader.get_categories(cats, 32);
	for (int i = 0; i < num; ++i) {
		WaveDefinition def;
		reader.get_int(cats[i], "cube_type", &def.cubeType);
		reader.get_int(cats[i], "max_concurrent", &def.maxConcurrent);
		reader.get_int(cats[i], "num_spawn", &def.numSpawn);
		reader.get_float(cats[i], "spawn_ttl", &def.spawnTTL);
		_definitions.push_back(def);
	}
	return true;
}

// ---------------------------------------
// Cubes
// ---------------------------------------
Cubes::Cubes(GameContext* context) : _context(context) , _world(context->world) {

	_spawnData.count_x = 10;
	_spawnData.count_y = 5;
	_spawnData.border = v2(120, 120);
	_spawnData.sides = 10;
	_spawnData.delay = 0.5f;
	_spawnData.type = SPT_PARTIAL_EDGES;
	_spawnData.emitter_type = SET_DELAYED;
	_spawnData.world_size = v2(1600, 900);
	_emitter = new BallEmitter(_spawnData);

	_cubeDefintions.load();
	_waveDefinitions.load();
	for (size_t i = 0; i < _waveDefinitions.size(); ++i) {
		WaveRuntime rt;
		rt.current = 0;
		rt.definitionIndex = i;
		rt.timer = 0.0f;
		rt.total = 0;
		_waveRuntimes.push_back(rt);
	}

}

Cubes::~Cubes() {
	delete _emitter;

}

// ---------------------------------------
// create ball
// ---------------------------------------
void Cubes::createBall(const v2& pos, int current, int total, int waveDefinitionIndex) {
	v2 position = pos;
	const WaveDefinition& waveDefinition = _waveDefinitions.get(waveDefinitionIndex);
	const CubeDefinition cubeDefinition = _cubeDefintions.get(waveDefinition.cubeType);
	if (total > 1) {
		float ra = static_cast<float>(current) / static_cast<float>(total)* TWO_PI;
		float rd = math::random(10.0f, 25.0f);
		v2 pp = v2(rd * cos(ra), rd * sin(ra));
		position += pp;
	}
	ds::SID sid = _world->create(position, cubeDefinition.name);
	_world->attachCollider(sid, cubeDefinition.type, 0);
	Ball* data = (Ball*)_world->attach_data(sid, sizeof(Ball));
	assert(data != 0);
	float angle = math::random(0.0f, TWO_PI);
	data->velocity = ds::vector::getRadialVelocity(angle, math::random(cubeDefinition.velocity - cubeDefinition.velocityVariance, cubeDefinition.velocity + cubeDefinition.velocityVariance));
	_world->scaleByPath(sid, &_context->settings->starScalePath,0.4f);
	data->force = v2(0, 0);
	data->def_index = waveDefinition.cubeType;
	data->wave_index = waveDefinitionIndex;
}

// ---------------------------------------
// handle events
// ---------------------------------------
void Cubes::handleEvents(const ds::ActionEventBuffer& buffer) {
	for (int i = 0; i < buffer.events.size(); ++i) {
		const ds::ActionEvent& event = buffer.events[i];
		if (_world->contains(event.sid)) {
			int t = _world->getType(event.sid);
			if (t == OT_BIG_CUBE || t == OT_HUGE_CUBE) {
				if (event.type == ds::AT_SCALE_BY_PATH) {
					Ball* data = (Ball*)_world->get_data(event.sid);
					assert(data != 0);
					_world->moveBy(event.sid, data->velocity, true);
				}
			}
		}
	}
}

// ---------------------------------------
// seek
// ---------------------------------------
void Cubes::seek(const v2& target, float velocity) {
	ds::SID sids[256];
	int num = _world->find_by_type(OT_FOLLOWER, sids, 256);
	for (int i = 0; i < num; ++i) {
		Ball* data = (Ball*)_world->get_data(sids[i]);
		assert(data != 0);
		v2 diff = target - _world->getPosition(sids[i]);
		v2 n = normalize(diff);
		v2 desired = n * velocity;
		data->force += desired;
	}
}

// ------------------------------------------------
// separate
// ------------------------------------------------
void Cubes::separate(const v2& target, float minDistance, float relaxation) {
	float sqrDist = minDistance * minDistance;
	ds::SID sids[256];
	int num = _world->find_by_type(OT_FOLLOWER, sids, 256);
	for (int i = 0; i < num; ++i) {
		Ball* data = (Ball*)_world->get_data(sids[i]);
		assert(data != 0);
		int cnt = 0;
		v2 separationForce = v2(0, 0);
		v2 averageDirection = v2(0, 0);
		v2 distance = v2(0, 0);
		v2 currentPos = _world->getPosition(sids[i]);
		for (int j = 0; j < num; j++) {
			if (i != j) {
				v2 dist = _world->getPosition(sids[j]) - currentPos;
				if (sqr_length(dist) < sqrDist) {
					++cnt;
					separationForce += dist;
					separationForce = normalize(separationForce);
					separationForce = separationForce * relaxation;
					averageDirection += separationForce;
				}
			}
		}
		if (cnt > 0) {
			data->force -= averageDirection;
		}
	}
}

// ------------------------------------------------
// align
// ------------------------------------------------
void Cubes::align(const v2& target, float desiredDistance) {
	float sqrDesired = desiredDistance * desiredDistance;
	ds::SID sids[256];
	int num = _world->find_by_type(OT_FOLLOWER, sids, 256);
	for (int i = 0; i < num; ++i) {
		Ball* data = (Ball*)_world->get_data(sids[i]);
		assert(data != 0);
		int cnt = 0;
		v2 separationForce = v2(0, 0);
		v2 averageDirection = v2(0, 0);
		v2 distance = v2(0, 0);
		v2 currentPos = _world->getPosition(sids[i]);
		for (int j = 0; j < num; j++) {
			if (i != j) {
				v2 dist = _world->getPosition(sids[j]) - currentPos;
				if (sqr_length(dist) < sqrDesired) {
					++cnt;
					averageDirection += data->velocity;
				}
			}
		}
		if (cnt > 0) {
			averageDirection /= static_cast<float>(cnt);
			data->force += averageDirection;
		}
	}
}
// ---------------------------------------
// move balls
// ---------------------------------------
void Cubes::move(float dt) {
	int count = 0;
	ds::SID sids[256];
	int num = _world->find_by_type(OT_FOLLOWER, sids, 256);
	for (int i = 0; i < num; ++i) {
		Ball* data = (Ball*)_world->get_data(sids[i]);
		assert(data != 0);
		data->force = v2(0, 0);
	}
	seek(_context->world_pos, 120.0f);
	separate(_context->world_pos, 40.0f, 15.0f);
	align(_context->world_pos, 40.0f);
	for (int i = 0; i < num; ++i) {
		Ball* data = (Ball*)_world->get_data(sids[i]);
		assert(data != 0);
		v2 p = _world->getPosition(sids[i]);
		p += data->force * dt;
		_world->setPosition(sids[i],p);
		v2 diff = _context->world_pos - p;
		v2 n = normalize(diff);
		_world->setRotation(sids[i], ds::vector::calculateRotation(n));
	}
}

// ---------------------------------------
// kill balls in range
// ---------------------------------------
int Cubes::killBalls(const v2& bombPos, KilledBall* killedBalls) {
	int count = 0;
	int types[] = { OT_FOLLOWER, OT_BIG_CUBE, OT_HUGE_CUBE };
	ds::SID sids[256];
	int num = _world->find_by_types(types, 3, sids, 256);
	for (int i = 0; i < num; ++i) {
		v2 p = _world->getPosition(sids[i]);
		Ball* data = (Ball*)_world->get_data(sids[i]);
		assert(data != 0);
		// FIXME!!!
		float size = 20.0f;
		if (ds::math::checkCircleIntersection(bombPos, BOMB_EXPLOSION_RADIUS, p, size)) {
			WaveRuntime& runtime = _waveRuntimes[data->wave_index];
			--runtime.current;
			KilledBall& kb = killedBalls[count++];
			kb.position = p;
			kb.type = _world->getType(sids[i]);
			_world->remove(sids[i]);
		}
	}
	return count;
}

// ---------------------------------------
// kill all
// ---------------------------------------
void Cubes::killAll(bool explode) {
	int types[] = { 4, 5, 6 };
	ds::SID sids[256];
	int num = _world->find_by_types(types, 3, sids, 256);
	for ( int i = 0; i < num; ++i) {
		if (explode) {
			v2 p = _world->getPosition(sids[i]);
			_context->particles->start(BALL_EXPLOSION, p);
		}
		_world->remove(sids[i]);
	}
}

// ------------------------------------------------
// emitt
// ------------------------------------------------
void Cubes::emitt(int type) {
	const SpawnPoint& spawn = _emitter->random();
	const WaveDefinition& waveDef = _waveDefinitions.get(type);
	for (int i = 0; i < waveDef.numSpawn; ++i) {
		createBall(spawn.position, i, waveDef.numSpawn, type);
	}
}

// ------------------------------------------------
// tick and create new cubes
// ------------------------------------------------
void Cubes::spawn(float dt) {
	for (size_t i = 0; i < _waveDefinitions.size(); ++i) {
		const WaveDefinition& waveDef = _waveDefinitions.get(i);
		WaveRuntime& runtime = _waveRuntimes[i];
		runtime.timer += dt;
		if (runtime.timer >= waveDef.spawnTTL) {
			runtime.timer = 0.0f;
			int delta = waveDef.maxConcurrent - runtime.current;
			if (delta > 0) {
				const SpawnPoint& spawn = _emitter->random();				
				for (int j = 0; j < waveDef.numSpawn; ++j) {
					createBall(spawn.position, j, waveDef.numSpawn, i);
					++runtime.total;
					++runtime.current;
				}
			}
		}

	}
}

// ------------------------------------------------
// activate
// ------------------------------------------------
void Cubes::activate() {
	_emitted = 0;
	// FIXME:
	killAll(false);
	_emitter->rebuild();
	_spawner_position = v2(200, 200);
	for (size_t i = 0; i < _waveRuntimes.size(); ++i) {
		WaveRuntime& rt = _waveRuntimes[i];
		rt.current = 0;
		rt.timer = 0.0f;
		rt.total = 0;
	}
}
