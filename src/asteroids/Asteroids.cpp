#include "Asteroids.h"
#include "WarpingGrid.h"
#include <particles\modules\RingEmitterModule.h>

const v2 CORNERS[] = { v2(40,90),v2(640,90),v2(1240,90),v2(1240,360),v2(1240,630),v2(640,630),v2(40,630),v2(40,360) };

Asteroids::Asteroids(GameContext* context) : _context(context) {
	_definitions.load();
	_scale_path.add(0.0f, v3(0.8f, 0.8f, 0.0f));
	_scale_path.add(0.5f, v3(1.1f, 1.1f, 0.0f));
	_scale_path.add(0.75f, v3(0.9f, 0.9f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
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
void Asteroids::startAsteroid(int type) {
	const AsteroidInfo& info = _definitions.getDefinition(type);
	int idx = math::random(0, 7);
	float start = (idx + 1) * DEGTORAD(45.0f) - DEGTORAD(22.5f);
	float end = (idx + 1) * DEGTORAD(45.0f) + DEGTORAD(22.5f);
	float angle = math::random(start, end);
	v2 p = CORNERS[idx];
	float r = info.radius * 0.2f;
	math::addRadial(p, r, angle);
	startAsteroid(type, p, angle);
}

// -------------------------------------------------------
// start asteroid
// -------------------------------------------------------
void Asteroids::startAsteroid(int type, const v2& pos, float angle) {
	const AsteroidInfo& info = _definitions.getDefinition(type);
	ID id = _context->world->create(pos, info.objectHash);	
	float ttl = math::random(0.2f, 0.4f);
	_context->world->scaleByPath(id, &_scale_path, ttl);
	AsteroidData* data = (AsteroidData*)_context->world->attach_data(id, sizeof(AsteroidData), type);
	data->energy = info.energy;
	data->type = type;
	data->startAngle = angle;
}

// -------------------------------------------------------
// handle event
// -------------------------------------------------------
void Asteroids::handleEvent(const ds::ActionEvent& event) {
	if (event.action == ds::AT_SCALE_BY_PATH) {
		if (event.type == OT_HUGE_ASTEROID || event.type == OT_BIG_ASTEROID || event.type == OT_MEDIUM_ASTEROID || event.type == OT_SMALL_ASTEROID) {
			AsteroidData* data = (AsteroidData*)_context->world->get_data(event.id);
			const AsteroidInfo& info = _definitions.getDefinition(data->type);
			v2 vel = math::getRadialVelocity(data->startAngle, math::random(info.minVelocity, info.maxVelocity));
			_context->world->moveBy(event.id, vel);
			_context->world->rotate(event.id, v3(info.rotationVelocity, 0.0f, 0.0f), -1.0f);
			_context->world->attachCollider(event.id, ds::ShapeType::PST_CIRCLE, v2(info.radius));
		}
	}
}

// -------------------------------------------------------
// start particles
// -------------------------------------------------------
void Asteroids::startParticles(int id, const AsteroidInfo& info, const v2& pos) {
	ds::ParticleSystem* system = _context->particles->getParticleSystem(id);
	ds::ParticleSpawner* spawner = system->getSpawner();
	spawner->rate = info.particles;
	ds::RingEmitterModuleData* emitter = (ds::RingEmitterModuleData*)system->getData(ds::ParticleModuleType::PM_RING);
	emitter->radius = info.radius * 0.5f;
	_context->particles->start(id, pos);
}

// -------------------------------------------------------
// kill
// -------------------------------------------------------
bool Asteroids::kill(ID id) {
	if (_context->world->contains(id)) {
		AsteroidData* data = (AsteroidData*)_context->world->get_data(id);
		--data->energy;
		if (data->energy <= 0) {
			const AsteroidInfo& info = _definitions.getDefinition(data->type);
			v3 p = _context->world->getPosition(id);

			startParticles(ASTEROID_EXPLOSION, info, p.xy());
			startParticles(ASTEROID_STREAKS, info, p.xy());

			_context->grid->applyForce(p.xy(), 0.1f, info.radius - 20.0f, info.radius + 20.0f);
			splitAsteroid(id);
			_context->world->remove(id);
			return true;
		}
	}
	return false;
}

// -------------------------------------------------------
// get info
// -------------------------------------------------------
const AsteroidInfo& Asteroids::getInfo(ID id) const {
	AsteroidData* data = (AsteroidData*)_context->world->get_data(id);
	return _definitions.getDefinition(data->type);
}

