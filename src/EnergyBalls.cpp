#include "EnergyBalls.h"
#include <math\Bitset.h>

const char* TEMPLATE_NAMES[] = { "Follower", "BigCube", "HugeCube" };

EnergyBalls::EnergyBalls(GameContext* context) : _context(context) {

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
}

EnergyBalls::~EnergyBalls() {
	delete _emitter;

}

// ---------------------------------------
// build from template
// ---------------------------------------
bool EnergyBalls::buildFromTemplate(Ball* ball, const char* name) {
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
void EnergyBalls::createBall(const v2& pos, int current, int total, EnergyBallType type) {
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
	ball.velocity = ds::vector::getRadialVelocity(angle, ds::math::random(20.0f, 40.0f));
	ball.state = Ball::BS_GROWING;
	ball.timer = 0.0f;
	ds::bit::set(&ball.behaviors, SIMPLE_MOVE_BIT);
	ball.type = type;
	ball.force = v2(0, 0);
	//ds::Sprite sprite;
	bool ret = buildFromTemplate(&ball, TEMPLATE_NAMES[type]);
	assert(ret);
	if (type == EBT_BIG_CUBE) {
		ball.velocity = ds::vector::getRadialVelocity(angle, ds::math::random(50.0f, 80.0f));
	}
	else if (type == EBT_HUGE_CUBE){
		ball.velocity = ds::vector::getRadialVelocity(angle, ds::math::random(20.0f, 50.0f));
	}
	
}

// ---------------------------------------
// render
// ---------------------------------------
void EnergyBalls::render() {
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		ds::sprites::draw(_balls.objects[i]);
	}
}

// ---------------------------------------
// scale growing balls
// ---------------------------------------
void EnergyBalls::scaleGrowingBalls(float dt) {
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		Ball& ball = _balls.objects[i];
		if (ball.state == Ball::BS_GROWING) {
			ball.timer += dt;
			float norm = ball.timer / _context->settings->ballGrowTTL;
			if (norm > 1.0f) {
				norm = 1.0f;
				ball.state = Ball::BS_MOVING;
				ball.timer = 0.0f;				
				if (ball.type == EBT_FOLLOWER) {
					ball.behaviors = 0;
					ds::bit::set(&ball.behaviors, SEEK_BIT);
					ds::bit::set(&ball.behaviors, SEPARATE_BIT);
				}
			}
			ball.color.a = norm;
			ball.scale = tweening::interpolate(tweening::easeInQuad, v2(0.1f, 0.1f), v2(1.0f, 1.0f), norm);
		}
	}
}

// ---------------------------------------
// move balls
// ---------------------------------------
void EnergyBalls::moveBalls(float dt) {
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
bool EnergyBalls::checkBallsInterception() const {
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
int EnergyBalls::killBalls(const v2& bombPos, KilledBall* killedBalls) {
	int count = 0;
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		if (ds::math::checkCircleIntersection(bombPos, BOMB_EXPLOSION_RADIUS, b.position, b.size)) {
			--_definitions[b.type].current;
			if (b.type == EBT_FOLLOWER) {
				// FIXME: count kills
				//++_killed;
				--_active_balls;
			}
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
void EnergyBalls::move(float dt) {
	// growing
	scaleGrowingBalls(dt);
	// move
	moveBalls(dt);
}

// ---------------------------------------
// kill all
// ---------------------------------------
void EnergyBalls::killAll() {
	for (uint32 i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		_context->particles->start(BALL_EXPLOSION, v3(b.position));
	}
	_balls.clear();
}

void EnergyBalls::emitt(EnergyBallType type, int count) {
	const SpawnPoint& spawn = _emitter->random();
	// 
	for (int i = 0; i < count; ++i) {
		createBall(spawn.position, i, count, type);
	}
}

void EnergyBalls::tick(BallDefinition& definition, float dt) {
	definition.timer += dt;
	if (definition.timer >= definition.spawnTTL) {
		definition.timer = 0.0f;
		int delta = definition.maxConcurrent - definition.current;
		if (delta > 0) {
			const SpawnPoint& spawn = _emitter->random();
			for (int i = 0; i < definition.num_spawn; ++i) {
				createBall(spawn.position, i, definition.num_spawn, definition.type);
				++definition.total;
				++definition.current;
			}
		}
	}
}
// ------------------------------------------------
// tick and create new dodgers
// ------------------------------------------------
void EnergyBalls::tick(float dt) {

	for (int i = 0; i < 3; ++i) {
		tick(_definitions[i], dt);
	}
	/*
	_ball_timer += dt;
	if (_ball_timer >= _context->playSettings->ballEmittTime) {		
		if (_context->playSettings->maxConcurrentBalls - _active_balls  > _context->playSettings->spawnBalls) {
			_ball_timer = 0.0f;
			const SpawnPoint& spawn = _emitter->random();
			for (int i = 0; i < _context->playSettings->spawnBalls; ++i) {
				createBall(spawn.position, i, _context->playSettings->spawnBalls, EBT_FOLLOWER);
				++_emitted;
				++_active_balls;
			}
		}
	}
	*/
	/*
	_big_ball_timer += dt;
	if (_big_ball_timer >= _context->playSettings->bigBallEmittTime) {
		_big_ball_timer = 0.0f;// -= _level_data.bigBallEmittTime;
		const SpawnPoint& spawn = _emitter->random();
		createBall(spawn.position, 1, 1, EBT_BIG_CUBE);
	}

	_huge_ball_timer += dt;
	if (_huge_ball_timer >= _context->playSettings->hugeBallEmittTime) {
		_huge_ball_timer = 0.0f;// -= _level_data.hugeBallEmittTime;
		const SpawnPoint& spawn = _emitter->random();
		createBall(spawn.position, 1, 1, EBT_HUGE_CUBE);
	}
	*/
	move(dt);
}

// ------------------------------------------------
// activate
// ------------------------------------------------
void EnergyBalls::activate() {
	_active_balls = 0;
	_emitted = 0;
	_balls.clear();
	_spawn_timer = 0.0f;
	_spawn_delay = 2.0f;
	_counter = 0;
	_maxBalls = 20;
	_emitter->rebuild();
	_spawner_position = v2(200, 200);

	_definitions[0].current = 0;
	_definitions[0].maxConcurrent = 100;
	_definitions[0].timer = 1000.0f;
	_definitions[0].total = 0;

	_definitions[0].num_spawn = 18;
	_definitions[0].spawnTTL = _context->playSettings->ballEmittTime;
	_definitions[0].type = EBT_FOLLOWER;

	_definitions[1].current = 0;
	_definitions[1].maxConcurrent = 80;
	_definitions[1].timer = 1000.0f;
	_definitions[1].total = 0;
	_definitions[1].num_spawn = 1;
	_definitions[1].spawnTTL = _context->playSettings->bigBallEmittTime;
	_definitions[1].type = EBT_BIG_CUBE;

	_definitions[2].current = 0;
	_definitions[2].maxConcurrent = 60;
	_definitions[2].timer = 1000.0f;
	_definitions[2].total = 0;
	_definitions[2].num_spawn = 1;
	_definitions[2].spawnTTL = _context->playSettings->hugeBallEmittTime;
	_definitions[2].type = EBT_HUGE_CUBE;
	/*
	_level_data.maxConcurrentBalls = 80;
	_level_data.spawnBalls = 10;
	_level_data.ballEmittTime = 2.0f;
	_level_data.bigBallEmittTime = 2.0f;
	_level_data.hugeBallEmittTime = 4.0f;
	*/
	_big_ball_timer = _context->playSettings->ballEmittTime;
	_huge_ball_timer = 0.0f;
	_ball_timer = _context->playSettings->ballEmittTime;

	_timers[1] = _context->playSettings->ballEmittTime;
	_timers[2] = 0.0f;
	_timers[0] = _context->playSettings->ballEmittTime;
}
