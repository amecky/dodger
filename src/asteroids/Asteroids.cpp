#include "Asteroids.h"


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
