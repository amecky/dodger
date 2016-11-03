#include "Asteroids.h"

const v2 CORNERS[] = { v2(40,90),v2(640,90),v2(1240,90),v2(1240,360),v2(1240,630),v2(640,630),v2(40,630),v2(40,360) };

Asteroids::Asteroids(GameContext* context) : _context(context) {
	_definitions.load();
}


Asteroids::~Asteroids() {
}

// -------------------------------------------------------
// split asteroid
// -------------------------------------------------------
void Asteroids::splitAsteroid(ID id) {
	AsteroidData* data = (AsteroidData*)_context->world->get_data(id);
	const AsteroidInfo& info = _definitions.getDefinition(data->type);
	if (data->type < 3) {
		v3 r = _context->world->getRotation(id);
		v3 p = _context->world->getPosition(id);
		float angle = r.x + HALF_PI;
		v2 np = p.xy();
		math::addRadial(np, 60.0f, angle);
		startAsteroid(data->type + 1, np, angle);
		angle += PI;
		np = p.xy();
		math::addRadial(np, 60.0f, angle);
		startAsteroid(data->type + 1, np, angle);
	}
}

void Asteroids::startAsteroid(int type) {
	const AsteroidInfo& info = _definitions.getDefinition(type);
	int idx = math::random(0, 7);
	float start = (idx + 1) * DEGTORAD(45.0f) - DEGTORAD(22.5f);
	float end = (idx + 1) * DEGTORAD(45.0f) + DEGTORAD(22.5f);
	float angle = math::random(start, end);
	//LOG << "idx: " << idx << " start: " << RADTODEG(start) << " end: " << RADTODEG(end);
	v2 p = CORNERS[idx];
	float r = info.radius + math::random(0.0f, 0.4f) * info.radius;
	math::addRadial(p, r, angle);
	//LOG << "angle: " << RADTODEG(angle) << " pos: " << DBG_V2(p);
	startAsteroid(type, p, angle);
}
// -------------------------------------------------------
// start asteroid
// -------------------------------------------------------
void Asteroids::startAsteroid(int type, const v2& pos, float angle) {
	const AsteroidInfo& info = _definitions.getDefinition(type);
	ID id = _context->world->create(pos, info.objectHash);
	v2 vel = math::getRadialVelocity(angle, math::random(info.minVelocity, info.maxVelocity));
	_context->world->moveBy(id, vel);
	//_context->world->scaleAxes(id, 0, 1.0f, 0.8f, 1.5f, -1, tweening::easeSinus);
	//_context->world->scaleAxes(id, 1, 1.0f, 1.2f, 2.5f, -1, tweening::easeSinus);
	_context->world->attachCollider(id, ds::ShapeType::PST_CIRCLE, v2(info.radius));
	AsteroidData* data = (AsteroidData*)_context->world->attach_data(id, sizeof(AsteroidData), type);
	data->energy = info.energy;
	data->type = type;
}

bool Asteroids::kill(ID id) {
	if (_context->world->contains(id)) {
		AsteroidData* data = (AsteroidData*)_context->world->get_data(id);
		--data->energy;
		if (data->energy <= 0) {
			const AsteroidInfo& info = _definitions.getDefinition(data->type);
			v3 p = _context->world->getPosition(id);
			_context->particles->start(info.particlesID, p.xy());
			splitAsteroid(id);
			_context->world->remove(id);
			return true;
		}
	}
	return false;
}



v2 Asteroids::pickStartPoint(float minRadius) {
	int idx = math::random(0, 7);
	float start = (idx + 1) * DEGTORAD(45.0f) - DEGTORAD(22.5f);
	float end = (idx + 1) * DEGTORAD(45.0f) + DEGTORAD(22.5f);
	float angle = math::random(start, end);
	LOG << "idx: " << idx << " start: " << RADTODEG(start) << " end: " << RADTODEG(end);
	v2 p = CORNERS[idx];
	float r = minRadius + math::random(0.0f, 0.4f) * minRadius;
	math::addRadial(p, r, angle);
	LOG << "angle: " << RADTODEG(angle) << " pos: " << DBG_V2(p);
	return p;
}
