#include "EnergyBalls.h"
#include <math\Bitset.h>

EnergyBalls::EnergyBalls(GameContext* context) : _context(context) {

	_spawnData.count_x = 10;
	_spawnData.count_y = 5;
	_spawnData.border = v2(40, 40);
	_spawnData.sides = 10;
	_spawnData.delay = 0.5f;
	_spawnData.type = SPT_PARTIAL_EDGES;
	_spawnData.emitter_type = SET_DELAYED;

	_emitter = new BallEmitter(_spawnData);
}

EnergyBalls::~EnergyBalls() {
	delete _emitter;

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
	ds::bit::set(&ball.behaviors, SEEK_BIT);
	ds::bit::set(&ball.behaviors, SEPARATE_BIT);
}

// ---------------------------------------
// render
// ---------------------------------------
void EnergyBalls::render() {
	for (int i = 0; i < _balls.numObjects; ++i) {
		const Ball& b = _balls.objects[i];
		ds::sprites::draw(b.position, ds::math::buildTexture(40, 280, 40, 40), b.rotation, b.scale.x, b.scale.y,ds::Color(90,184,196,255));
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
			}
			ball.scale = tweening::interpolate(tweening::easeInQuad, v2(0.1f, 0.1f), v2(1.0f, 1.0f), norm);
		}
	}
}

// ---------------------------------------
// move starting balls
// ---------------------------------------
void EnergyBalls::moveStartingBalls(float dt) {
	for (int i = 0; i < _balls.numObjects; ++i) {
		Ball& ball = _balls.objects[i];
		if (ball.state == Ball::BS_GROWING) {
			ball.position += ball.velocity * dt;
		}
	}
}

// ---------------------------------------
// move balls
// ---------------------------------------
void EnergyBalls::moveBalls(float dt) {
	// reset velocity
	for (int i = 0; i < _balls.numObjects; ++i) {
		if (_balls.objects[i].state == Ball::BS_MOVING) {
			_balls.objects[i].velocity = v2(0, 0);
		}
	}
	// apply behaviors
	behavior::seek(_balls.objects, _balls.numObjects, _context->playerPosition, 60.0f, dt);
	behavior::separate(_balls.objects, _balls.numObjects, _context->playerPosition, 40.0f, 15.0f, dt);
	behavior::align(_balls.objects, _balls.numObjects, _context->playerPosition, 40.0f, dt);
	// move and rotate
	for (int i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		b.position += b.velocity * dt;
		v2 diff = _context->playerPosition - b.position;
		v2 n = normalize(diff);
		b.rotation = ds::vector::calculateRotation(n);
	}
}

// ---------------------------------------
// check balls interception with player
// ---------------------------------------
bool EnergyBalls::checkBallsInterception() const {
	for (int i = 0; i < _balls.numObjects; ++i) {
		const Ball& b = _balls.objects[i];
		if (ds::math::checkCircleIntersection(_context->playerPosition, PLAYER_RADIUS, b.position, 15.0f)) {
			return true;
		}
	}
	return false;
}

// ---------------------------------------
// kill balls in range
// ---------------------------------------
int EnergyBalls::killBalls(const v2& bombPos,v2* positions) {
	int count = 0;
	for (int i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		if (ds::math::checkCircleIntersection(bombPos, BOMB_EXPLOSION_RADIUS, b.position, 15.0f)) {
			positions[count++] = b.position;
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
	// starting
	moveStartingBalls(dt);
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

// ------------------------------------------------
// tick and create new dodgers
// ------------------------------------------------
void EnergyBalls::tick(float dt) {
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
}
