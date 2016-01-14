#include "Cubes.h"
#include <math\Bitset.h>

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

Cubes::Cubes(GameContext* context) : _context(context) {

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
// build from template
// ---------------------------------------
bool Cubes::buildFromTemplate(Ball* ball, const char* name) {
	ds::Sprite sprite;
	if (ds::renderer::getSpriteTemplate(name, &sprite)) {
		ball->texture = sprite.texture;
		ball->color = sprite.color;
		float dim = sprite.texture.rect.width();
		if (sprite.texture.rect.height() > dim) {
			dim = sprite.texture.rect.height();
		}
		ball->size = dim * 0.5f;
		return true;
	}
	return false;
}

// ---------------------------------------
// create ball
// ---------------------------------------
void Cubes::createBall(const v2& pos, int current, int total, const CubeDefinition& cubeDefinition) {
	ID id = _balls.add();
	Ball& ball = _balls.get(id);
	ball.position = pos;
	if (total > 1) {
		float ra = static_cast<float>(current) / static_cast<float>(total)* TWO_PI;
		float rd = ds::math::random(10.0f, 25.0f);
		v2 pp = v2(rd * cos(ra), rd * sin(ra));
		ball.position += pp;
	}
	float angle = ds::math::random(0.0f, TWO_PI);
	ball.velocity = ds::vector::getRadialVelocity(angle, ds::math::random(cubeDefinition.velocity - cubeDefinition.velocityVariance, cubeDefinition.velocity + cubeDefinition.velocityVariance));
	ball.state = Ball::BS_GROWING;
	ball.timer = 0.0f;
	ds::bit::set(&ball.behaviors, SIMPLE_MOVE_BIT);
	ball.type = cubeDefinition.type;
	ball.force = v2(0, 0);
	bool ret = buildFromTemplate(&ball, cubeDefinition.name);
	assert(ret);
}

// ---------------------------------------
// render
// ---------------------------------------
void Cubes::render() {
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		ds::sprites::draw(_balls.objects[i]);
	}
}

// ---------------------------------------
// scale growing balls
// ---------------------------------------
void Cubes::scaleGrowingBalls(float dt) {
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		Ball& ball = _balls.objects[i];
		if (ball.state == Ball::BS_GROWING) {
			const CubeDefinition& def = _cubeDefintions.get(ball.type);
			ball.timer += dt;
			float norm = ball.timer / def.growTTL;
			if (norm > 1.0f) {
				norm = 1.0f;
				ball.state = Ball::BS_MOVING;
				ball.timer = 0.0f;
				ball.behaviors = def.behaviorBits;
			}
			ball.color.a = norm;
			ball.scale = tweening::interpolate(tweening::easeInQuad, v2(0.1f, 0.1f), v2(1.0f, 1.0f), norm);
		}
	}
}

// ---------------------------------------
// move balls
// ---------------------------------------
void Cubes::moveBalls(float dt) {
	// reset velocity
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		_balls.objects[i].force = v2(0, 0);
	}
	// apply behaviors
	behavior::seek(_balls.objects, _balls.numObjects, _context->world_pos, 120.0f, dt);
	behavior::separate(_balls.objects, _balls.numObjects, _context->world_pos, 40.0f, 15.0f, dt);
	behavior::align(_balls.objects, _balls.numObjects, _context->world_pos, 40.0f, dt);
	behavior::simple_move(_balls.objects, _balls.numObjects, dt);
	// move and rotate
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		b.position += b.force * dt;
		bool changed = false;
		if (b.position.x < 100.0f || b.position.x > 1500.0f) {
			b.velocity.x *= -1.0f;
			changed = true;
		}
		if (b.position.y < 50.0f || b.position.y > 850.0f) {
			b.velocity.y *= -1.0f;
			changed = true;
		}
		if (changed) {
			b.position += b.velocity * dt;
		}
		v2 diff = _context->world_pos - b.position;
		v2 n = normalize(diff);
		b.rotation = ds::vector::calculateRotation(n);
		// FIXME: make sure we are inside the grid!
	}
}

// ---------------------------------------
// check balls interception with player
// ---------------------------------------
bool Cubes::checkBallsInterception() const {
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		const Ball& b = _balls.objects[i];
		if (ds::math::checkCircleIntersection(_context->world_pos, PLAYER_RADIUS, b.position, 15.0f)) {
			return true;
		}
	}
	return false;
}

// ---------------------------------------
// kill balls in range
// ---------------------------------------
int Cubes::killBalls(const v2& bombPos, KilledBall* killedBalls) {
	int count = 0;
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		if (ds::math::checkCircleIntersection(bombPos, BOMB_EXPLOSION_RADIUS, b.position, b.size)) {
			WaveRuntime& runtime = _waveRuntimes[b.type];
			--runtime.current;
			KilledBall& kb = killedBalls[count++];
			kb.position = b.position;
			kb.type = b.type;
			_balls.remove(b.id);
		}
	}
	return count;
}

// ---------------------------------------
// move
// ---------------------------------------
void Cubes::move(float dt) {
	// growing
	scaleGrowingBalls(dt);
	// move
	moveBalls(dt);
}

// ---------------------------------------
// kill all
// ---------------------------------------
void Cubes::killAll() {
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		_context->particles->start(BALL_EXPLOSION, v3(b.position));
	}
	_balls.clear();
}

// ------------------------------------------------
// emitt
// ------------------------------------------------
void Cubes::emitt(int type) {
	const SpawnPoint& spawn = _emitter->random();
	const CubeDefinition& def = _cubeDefintions.get(type);
	const WaveDefinition& waveDef = _waveDefinitions.get(type);
	for (int i = 0; i < waveDef.numSpawn; ++i) {
		createBall(spawn.position, i, waveDef.numSpawn, def);
	}
}

// ------------------------------------------------
// tick and create new dodgers
// ------------------------------------------------
void Cubes::tick(float dt) {
	for (size_t i = 0; i < _waveDefinitions.size(); ++i) {
		const WaveDefinition& waveDef = _waveDefinitions.get(i);
		WaveRuntime& runtime = _waveRuntimes[i];
		runtime.timer += dt;
		if (runtime.timer >= waveDef.spawnTTL) {
			runtime.timer = 0.0f;
			int delta = waveDef.maxConcurrent - runtime.current;
			if (delta > 0) {
				const SpawnPoint& spawn = _emitter->random();
				const CubeDefinition cubeDef = _cubeDefintions.get(waveDef.cubeType);
				for (int i = 0; i < waveDef.numSpawn; ++i) {
					createBall(spawn.position, i, waveDef.numSpawn, cubeDef);
					++runtime.total;
					++runtime.current;
				}
			}
		}

	}
	move(dt);
}

// ------------------------------------------------
// activate
// ------------------------------------------------
void Cubes::activate() {
	_emitted = 0;
	_balls.clear();
	_emitter->rebuild();
	_spawner_position = v2(200, 200);
	for (size_t i = 0; i < _waveRuntimes.size(); ++i) {
		WaveRuntime& rt = _waveRuntimes[i];
		rt.current = 0;
		rt.timer = 0.0f;
		rt.total = 0;
	}
}
