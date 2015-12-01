#include "EnergyBalls.h"


EnergyBalls::EnergyBalls(GameContext* context) : _context(context) {

	_spawnData.count_x = 10;
	_spawnData.count_y = 5;
	_spawnData.border = v2(40, 40);
	_spawnData.sides = 10;
	_spawnData.delay = 0.5f;
	_spawnData.type = SPT_PARTIAL_EDGES;
	_spawnData.emitter_type = SET_DELAYED;

	_emitter = new BallEmitter(_spawnData);

	//createBall(v2(80,80));
	//createBall(v2(80, 160));
	//createBall(v2(80, 240));

	createGate(v2(180, 600));
	createGate(v2(680, 200), Gate::HORIZONTAL);

	_stars = new Stars(_context);
}

EnergyBalls::~EnergyBalls() {
	delete _stars;
	delete _emitter;

}

// ---------------------------------------
// create ball
// ---------------------------------------
void EnergyBalls::createBall(const v2& pos) {
	ID id = _balls.add();
	Ball& ball = _balls.get(id);
	ball.position = pos;
	ball.velocity = v2(0, 0);
	ball.state = Ball::BS_GROWING;
	ball.timer = 0.0f;// ds::math::random(0.0f, 0.4f);
}

// ---------------------------------------
// create gate
// ---------------------------------------
void EnergyBalls::createGate(const v2& pos, Gate::Orientation orientation) {
	// FIXME: check if we might have a inactive gate
	int gx = ds::math::random(0, 7);
	int gy = ds::math::random(0, 5);
	int or = ds::math::random(0, 1);
	float px = 75.0f + gx * 150.0f;
	float py = 60.0f + gy * 120.0f;
	v2 pp = v2(px, py);
	ID id = _gates.add();
	Gate& gate = _gates.get(id);
	gate.position = pp;
	gate.orientation = orientation;
	gate.state = Gate::ACTIVE;
	gate.timer = 0.0f;
	//if (orientation == Gate::HORIZONTAL) {
	float step = PI / 10.0f;
	if ( or == 0) {
		gate.orientation = Gate::HORIZONTAL;
		gate.aabBox = ds::AABBox(pos, v2(30, 100));		
	}
	else {
		gate.orientation = Gate::VERTICAL;
		gate.aabBox = ds::AABBox(pos, v2(100, 30));
	}
	for (int i = 0; i < 10; ++i) {
		gate.cell_scales[i] = step * static_cast<float>(i);
	}
}

// ---------------------------------------
// render
// ---------------------------------------
void EnergyBalls::render() {
	for (int i = 0; i < _balls.numObjects; ++i) {
		const Ball& b = _balls.objects[i];
		ds::sprites::draw(b.position, ds::math::buildTexture(0, 120, 24, 24), b.rotation, b.scale.x, b.scale.y);
	}
	_stars->render();
	for (int i = 0; i < _gates.numObjects; ++i) {
		const Gate& gate = _gates.objects[i];
		if (gate.state == Gate::FLASHING || gate.state == Gate::ACTIVE) {
			ds::sprites::draw(gate.position, ds::math::buildTexture(40, 40, 40, 40));
			/*
			v2 p = gate.position;
			if (gate.orientation == Gate::VERTICAL) {
				p.x -= 50.0f;
				ds::sprites::draw(p, ds::math::buildTexture(460, 180, 20, 20));
				p.x += 110.0f;
				ds::sprites::draw(p, ds::math::buildTexture(440, 180, 20, 20));
				p.x = gate.position.x - 45.0f;
				for (int d = 0; d < 10; ++d) {
					ds::sprites::draw(p, ds::math::buildTexture(440, 220, 10, 10));
					p.x += 10.0f;
				}
			}
			else {
				p.y -= 60.0f;
				ds::sprites::draw(p, ds::math::buildTexture(460, 200, 20, 20));
				p.y += 110.0f;
				ds::sprites::draw(p, ds::math::buildTexture(440, 200, 20, 20));
				p.y = gate.position.y - 45.0f;
				for (int d = 0; d < 10; ++d) {
					ds::sprites::draw(p, ds::math::buildTexture(440, 220, 10, 10));
					p.y += 10.0f;
				}
			}
			*/
			if (gate.state == Gate::FLASHING) {
				ds::sprites::draw(gate.position, ds::math::buildTexture(440, 0, 152, 152));
			}
		}
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
		if (ball.state == Ball::BS_STARTING) {
			ball.timer += dt;
			float norm = ball.timer / _context->settings->ballGrowTTL;
			if (norm > 1.0f) {
				norm = 1.0f;
				ball.state = Ball::BS_STARTING;
				ball.timer = 0.0f;
			}
			ball.scale = tweening::interpolate(tweening::easeInQuad, v2(0.1f, 0.1f), v2(1.0f, 1.0f), norm);
		}
	}
}

// ---------------------------------------
// move balls
// ---------------------------------------
void EnergyBalls::moveBalls(float dt) {
	for (int i = 0; i < _balls.numObjects; ++i) {
		if (_balls.objects[i].state == Ball::BS_MOVING) {
			_balls.objects[i].velocity = v2(0, 0);
		}
	}

	_seek.tick(_balls.objects, _balls.numObjects, _context->playerPosition, dt);
	_separation.tick(_balls.objects, _balls.numObjects, _context->playerPosition, dt);
	//_align.tick(_balls, _count, _context->playerPosition, dt);

	for (int i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		// get velocity
		b.position += b.velocity * dt;
		v2 diff = _context->playerPosition - b.position;
		v2 n = normalize(diff);
		b.rotation = ds::vector::calculateRotation(n);
	}
}

// ---------------------------------------
// scale gates
// ---------------------------------------
void EnergyBalls::scaleGates(float dt) {
	for (int i = 0; i < _gates.numObjects; ++i) {
		Gate& gate = _gates.objects[i];
		if (gate.state == Gate::FLASHING) {
			gate.timer += dt;
			float norm = gate.timer / _context->settings->gateFlashingTTL;
			if (norm > 1.0f) {
				// explode gate / kill balls in radius
				norm = 1.0f;
				gate.state = Gate::INACTIVE;
				killBalls(gate.position);
			}
			float s = 0.9f + sin(norm * 6.0f) * 0.1f;
			gate.scale.x = s;
			gate.scale.y = s;
			if (gate.state == Gate::INACTIVE) {
				_gates.remove(gate.id);
				createGate(v2(0, 0), Gate::VERTICAL);
			}
		}
	}
}

// ---------------------------------------
// check gate player interception
// ---------------------------------------
void EnergyBalls::checkGateInterception() {
	ds::Sphere sp;
	sp.radius = 24.0f;
	sp.position = v3(_context->playerPosition.x, _context->playerPosition.y, 0.0f);
	for (int i = 0; i < _gates.numObjects; ++i) {
		Gate& gate = _gates.objects[i];
		if (gate.state == Gate::ACTIVE) {
			if (ds::math::checkCircleIntersection(_context->playerPosition, PLAYER_RADIUS, gate.position, 20.0f)) {
			//if (gate.aabBox.overlaps(sp)) {
				gate.state = Gate::FLASHING;
				gate.timer = 0.0f;
			}
		}
	}
}

void EnergyBalls::checkBallsInterception() {
	for (int i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		if (ds::math::checkCircleIntersection(_context->playerPosition, PLAYER_RADIUS, b.position, 15.0f)) {
			LOG << "PLAYER GOT KILLED";
			// kill all balls
		}
	}
}

void EnergyBalls::killBalls(const v2& bombPos) {
	int count = 0;
	for (int i = 0; i < _balls.numObjects; ++i) {
		Ball& b = _balls.objects[i];
		if (ds::math::checkCircleIntersection(bombPos, 75.0f, b.position, 15.0f)) {
			_stars->add(b.position);
			_balls.remove(b.id);
			++count;
		}
	}
	LOG << "killed: " << count;
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
	// check gate and player interception
	checkGateInterception();
	// scale gates
	scaleGates(dt);
	// player to balls interception
	checkBallsInterception();
	// tick stars
	_stars->tick(dt);
	_stars->pickup(_context->playerPosition, PLAYER_RADIUS);
	_stars->move(_context->playerPosition, dt);
}

// ------------------------------------------------
// tick and create new dodgers
// ------------------------------------------------
void EnergyBalls::tick(float dt) {
	if (_spawnData.emitter_type == SET_DELAYED) {
		_spawnTimer += dt;
		if (_spawnTimer > _spawnData.delay) {
			_spawnTimer = 0.0f;
			if (_counter < _maxBalls){
				StartPoint sp;
				const SpawnPoint& spawn = _emitter->next();
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
	move(dt);
}

void EnergyBalls::activate() {
	_spawnTimer = 0.0f;
	_counter = 0;
	_maxBalls = 20;
	_emitter->rebuild();
}



void SeekBehavior::tick(Ball* balls, int count, const v2& target, float dt) {
	for (int i = 0; i < count; ++i) {
		if (balls[i].state == Ball::BS_MOVING) {
			v2 diff = target - balls[i].position;
			v2 n = normalize(diff);
			v2 desired = n * 150.0f;
			balls[i].velocity += desired;
		}
	}
}

void SeparationBehavior::tick(Ball* balls, int count, const v2& target, float dt) {
	for (int i = 0; i < count; ++i) {
		Ball& ball = balls[i];
		if (ball.state == Ball::BS_MOVING) {
			int cnt = 0;
			v2 separationForce = v2(0, 0);
			v2 averageDirection = v2(0, 0);
			v2 distance = v2(0, 0);
			for (int j = 0; j < count; j++) {
				if (i != j) {
					v2 dist = balls[j].position - ball.position;
					if (sqr_length(dist) < 1600.0f) {
						++cnt;
						separationForce += dist;
						separationForce = normalize(separationForce);
						separationForce = separationForce * 20.0f;// / 0.9f;
						averageDirection += separationForce;
					}
				}
			}
			if (cnt > 0) {
				balls[i].velocity -= averageDirection;
			}
		}
	}
}


void AlignBehavior::tick(Ball* balls, int count, const v2& target, float dt) {
	for (int i = 0; i < count; ++i) {
		Ball& ball = balls[i];
		if (ball.state == Ball::BS_MOVING) {
			int cnt = 0;
			v2 separationForce = v2(0, 0);
			v2 averageDirection = v2(0, 0);
			v2 distance = v2(0, 0);
			for (int j = 0; j < count; j++) {
				if (i != j) {
					v2 dist = balls[j].position - ball.position;
					if (sqr_length(dist) < 1600.0f) {
						++cnt;
						averageDirection += ball.velocity;
					}
				}
			}
			if (cnt > 0) {
				averageDirection /= static_cast<float>(cnt);
				balls[i].velocity += averageDirection;
			}
		}
	}
}