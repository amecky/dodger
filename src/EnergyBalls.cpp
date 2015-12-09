#include "EnergyBalls.h"
#include <math\Bitset.h>

EnergyBalls::EnergyBalls(GameContext* context) : _context(context) {

	_spawnData.count_x = 10;
	_spawnData.count_y = 5;
	_spawnData.border = v2(120, 120);
	_spawnData.sides = 10;
	_spawnData.delay = 0.5f;
	_spawnData.type = SPT_PARTIAL_EDGES;
	_spawnData.emitter_type = SET_DELAYED;
	_spawnData.world_size = v2(1920, 1080);
	_emitter = new BallEmitter(_spawnData);
}

EnergyBalls::~EnergyBalls() {
	delete _emitter;

}

void EnergyBalls::createBall(const v2& pos, int current, int total, EnergyBallType type) {
	ID id = _balls.add();
	Ball& ball = _balls.get(id);
	// FIXME: spread out with radius
	ball.position = pos;
	float angle = ds::math::random(0.0f, TWO_PI);
	ball.velocity = ds::vector::getRadialVelocity(angle, ds::math::random(10.0f, 20.0f));
	ball.state = Ball::BS_GROWING;
	ball.timer = 0.0f;
	ds::bit::set(&ball.behaviors, SIMPLE_MOVE_BIT);
	if (type == EBT_FOLLOWER) {
		ball.texture = ds::math::buildTexture(80, 160, 25, 25);
		ball.color = ds::Color(90, 184, 196, 255);
		ball.size = 12.0f;
	}
	else if (type == EBT_BIG_CUBE) {
		ball.texture = ds::math::buildTexture(80, 40, 40, 40);
		ball.color = ds::Color(200, 0, 120, 255);
		ball.size = 20.0f;
		ball.velocity = ds::vector::getRadialVelocity(angle, ds::math::random(50.0f, 80.0f));
	}
	else {
		ball.texture = ds::math::buildTexture(0, 200, 60, 60);
		ball.color = ds::Color(253, 138, 81, 255);
		ball.size = 30.0f;
		ball.velocity = ds::vector::getRadialVelocity(angle, ds::math::random(20.0f, 50.0f));
	}
	ball.type = type;
	ball.force = v2(0, 0);
}
// ---------------------------------------
// create ball
// ---------------------------------------
void EnergyBalls::createBall(const v2& pos) {
	ID id = _balls.add();
	Ball& ball = _balls.get(id);
	ball.position = pos;
	float angle = ds::math::random(0.0f, TWO_PI);
	ball.velocity = ds::vector::getRadialVelocity(angle, ds::math::random(10.0f, 20.0f));
	ball.state = Ball::BS_GROWING;
	ball.timer = 0.0f;
	ds::bit::set(&ball.behaviors, SIMPLE_MOVE_BIT);
	ball.texture = ds::math::buildTexture(80, 160, 25, 25);
	ball.color = ds::Color(90, 184, 196, 255);
	ball.force = v2(0, 0);
	ball.type = EBT_FOLLOWER;
}

// ---------------------------------------
// render
// ---------------------------------------
void EnergyBalls::render() {
	for (int i = 0; i < _balls.numObjects; ++i) {
		const Ball& b = _balls.objects[i];
		ds::sprites::draw(b.position, b.texture, b.rotation, b.scale.x, b.scale.y,b.color);
	}
}

// ---------------------------------------
// scale growing balls
// ---------------------------------------
void EnergyBalls::scaleGrowingBalls(float dt) {
	for (int i = 0; i < _balls.numObjects; ++i) {
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
			ball.scale = tweening::interpolate(tweening::easeInQuad, v2(0.1f, 0.1f), v2(1.0f, 1.0f), norm);
		}
	}
}

// ---------------------------------------
// move balls
// ---------------------------------------
void EnergyBalls::moveBalls(float dt) {
	// reset velocity
	for (int i = 0; i < _balls.numObjects; ++i) {
		_balls.objects[i].force = v2(0, 0);
	}
	// apply behaviors
	behavior::seek(_balls.objects, _balls.numObjects, _context->world_pos, 60.0f, dt);
	behavior::separate(_balls.objects, _balls.numObjects, _context->world_pos, 40.0f, 15.0f, dt);
	behavior::align(_balls.objects, _balls.numObjects, _context->world_pos, 40.0f, dt);
	behavior::simple_move(_balls.objects, _balls.numObjects, dt);
	// move and rotate
	for (int i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		b.position += b.force * dt;
		bool changed = false;
		if (b.position.x < 100.0f || b.position.x > 1820.0f) {
			b.velocity.x *= -1.0f;
			changed = true;
		}
		if (b.position.y < 50.0f || b.position.y > 1030.0f) {
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
	for (int i = 0; i < _balls.numObjects; ++i) {
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
	for (int i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		if (ds::math::checkCircleIntersection(bombPos, BOMB_EXPLOSION_RADIUS, b.position, b.size)) {
			if (b.type == EBT_FOLLOWER) {
				// FIXME: count kills
				//++_killed;
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
	for (int i = 0; i < _balls.numObjects; ++i) {
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
// ------------------------------------------------
// tick and create new dodgers
// ------------------------------------------------
void EnergyBalls::tick(float dt) {
	/*
	int delta = _balls.numObjects - _level_data.minBalls;
	if (delta < 0) {
		// pick start point
		const SpawnPoint& spawn = _emitter->random();
		int emittCount = _level_data.emittBalls;
		if (_level_data.emitted + emittCount >= _level_data.totalBalls) {
			emittCount = _level_data.totalBalls - _level_data.emitted;
		}
		for (int i = 0; i < emittCount; ++i) {
			createBall(spawn.position);
			++_level_data.emitted;
		}
	}
	_context->debugPanel.show("Total", _level_data.totalBalls);
	_context->debugPanel.show("Emitted", _level_data.emitted);
	_context->debugPanel.show("Count", _level_data.emittBalls);
	*/
	/*
	if (_spawnData.emitter_type == SET_DELAYED) {
		_spawn_timer += dt;
		if (_spawn_timer > _spawnData.delay) {
			_spawn_timer = 0.0f;
			if (_counter < _maxBalls){
				StartPoint sp;
				const SpawnPoint& spawn = _emitter->random();
				sp.position = spawn.position;
				sp.normal = spawn.normal;
				sp.timer = 0.0f;
				_startPoints.push_back(sp);
				// FIXME: call back for emitt particles
				//_context->particles.start(9, sp.position);
				_context->particles->start(2, sp.position);
				++_counter;
			}
		}
	}
	if (!_startPoints.empty()) {
		StartPoints::iterator it = _startPoints.begin();
		while (it != _startPoints.end()) {
			it->timer += dt;
			if (it->timer >= _context->settings->spawnDelay) {
				createBall(it->position);
				it = _startPoints.erase(it);
			}
			else {
				++it;
			}
		}
	}
	*/
	_ball_timer += dt;
	if (_ball_timer >= _level_data.bigBallEmittTime) {
		_ball_timer -= _level_data.bigBallEmittTime;
		if (_level_data.emitted + _level_data.spawnBalls < _level_data.totalBalls) {
			const SpawnPoint& spawn = _emitter->random();
			for (int i = 0; i < _level_data.spawnBalls; ++i) {
				createBall(spawn.position, i, _level_data.spawnBalls, EBT_FOLLOWER);
				++_level_data.emitted;
			}
		}
	}

	_big_ball_timer += dt;
	if (_big_ball_timer >= _level_data.bigBallEmittTime) {
		_big_ball_timer -= _level_data.bigBallEmittTime;
		const SpawnPoint& spawn = _emitter->random();
		createBall(spawn.position, 1, 1, EBT_BIG_CUBE);
	}

	_huge_ball_timer += dt;
	if (_huge_ball_timer >= _level_data.hugeBallEmittTime) {
		_huge_ball_timer -= _level_data.hugeBallEmittTime;
		const SpawnPoint& spawn = _emitter->random();
		createBall(spawn.position, 1, 1, EBT_HUGE_CUBE);
	}
	move(dt);
}

// ------------------------------------------------
// activate
// ------------------------------------------------
void EnergyBalls::activate() {
	_balls.clear();
	_spawn_timer = 0.0f;
	_spawn_delay = 2.0f;
	_counter = 0;
	_maxBalls = 20;
	_emitter->rebuild();
	_spawner_position = v2(200, 200);
	_level_data.totalBalls = 60;
	_level_data.emittBalls = 20;
	_level_data.minBalls = 10;
	_level_data.spawnBalls = 5;
	_level_data.emitted = 0;
	_level_data.bigBallEmittTime = 1.2f;
	_level_data.hugeBallEmittTime = 2.5f;
	_big_ball_timer = 0.0f;
	_huge_ball_timer = 0.0f;
	_ball_timer = 0.0f;
}
