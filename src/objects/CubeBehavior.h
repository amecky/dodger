#pragma once
#include <core\world\World.h>
#include "..\GameSettings.h"

class CubeBehavior {

public:
	CubeBehavior(ds::World* world, GameSettings* settings) : _world(world) , _settings(settings) {}
	virtual ~CubeBehavior() {}
	virtual void create(ID id) = 0;
	virtual void tick(const ds::ActionEvent& event, ID target, int objectType) = 0;
protected:
	ds::World* _world;
	GameSettings* _settings;
};

class BaseBehavior : public CubeBehavior {

public:
	BaseBehavior(ds::World* world, GameSettings* settings) : CubeBehavior(world, settings) {
		_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
		_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
		_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
		_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));
	}
	virtual ~BaseBehavior() {}
	virtual void create(ID id) {
		float ttl = math::random(0.5f, 0.8f);
		_world->scaleByPath(id, &_scale_path, ttl);
	}
	virtual void tick(const ds::ActionEvent& event, ID target, int objectType) {}
protected:
	ds::V3Path _scale_path;
};

class FollowerBehavior : public BaseBehavior {

public:
	FollowerBehavior(ds::World* world, GameSettings* settings) : BaseBehavior(world, settings) {}
	virtual ~FollowerBehavior() {}
	virtual void create(ID id) {
		float ttl = math::random(1.25f, 1.5f);
		//_world->scaleByPath(id, &_scale_path, ttl);
		_world->scale(id, v3(0.2f, 0.2f, 0.0f), v3(1.0f), ttl);
		v3 r = _world->getRotation(id);
		v2 vel = math::getRadialVelocity(r.x, 20.0f);
		_world->moveBy(id, vel, ttl);
	}
	virtual void tick(const ds::ActionEvent& event, ID target, int objectType) {
		if (event.action == ds::AT_SCALE) {
			_world->attachCollider(event.id, ds::PST_CIRCLE, v2(42.0f, 42.0f));
			_world->seek(event.id, target, 150.0f);
			_world->separate(event.id, objectType, 30.0f, 0.9f);
		}
	}
};

class SpotterBehavior : public BaseBehavior {

public:
	SpotterBehavior(ds::World* world, GameSettings* settings) : BaseBehavior(world, settings) {}
	virtual ~SpotterBehavior() {}
	virtual void tick(const ds::ActionEvent& event, ID target, int objectType) {
		if (event.action == ds::AT_SCALE_BY_PATH) {
			_world->lookAt(event.id, target, 1.0f);
			_world->attachCollider(event.id, ds::PST_CIRCLE, v2(48.0f, 48.0f));
		}
		else if (event.action == ds::AT_LOOK_AT) {
			v3 rot = _world->getRotation(event.id);
			v3 v = math::getRadialVelocity(rot.x, 150.0f);
			float ttl = math::random(4.0f, 8.0f);
			_world->moveBy(event.id, v, ttl);
		}
		else if (event.action == ds::AT_MOVE_BY) {
			_world->lookAt(event.id, target, 1.0f);
		}
	}
};

class WandererBehavior : public BaseBehavior {

public:
	WandererBehavior(ds::World* world, GameSettings* settings) : BaseBehavior(world, settings) {}
	virtual ~WandererBehavior() {}
	float getRotationAngle() {
		float angle = math::random(_settings->wanderer.minRotationAngle, _settings->wanderer.maxRotationAngle);
		int dir = math::random(0, 100);
		if (dir >= 50) {
			angle *= -1.0f;
		}
		return DEGTORAD(angle);
	}
	virtual void tick(const ds::ActionEvent& event, ID target, int objectType) {
		if (event.action == ds::AT_SCALE_BY_PATH) {
			float ttl = math::random(_settings->wanderer.minRotationTTL, _settings->wanderer.maxRotationTTL);
			_world->rotateBy(event.id, getRotationAngle(), ttl);
			_world->attachCollider(event.id, ds::PST_CIRCLE, v2(44.0f, 44.0f));
		}
		else if (event.action == ds::AT_ROTATE_BY) {
			v3 rot = _world->getRotation(event.id);
			float vel = math::random(_settings->wanderer.minVelocity, _settings->wanderer.maxVelocity);
			v3 v = math::getRadialVelocity(rot.x, vel);
			float ttl = math::random(_settings->wanderer.minMoveTTL, _settings->wanderer.maxMoveTTL);
			_world->moveBy(event.id, v, ttl);
			_world->scale(event.id, v3(1.0f, 1.0f, 1.0f), v3(0.9f, 0.9f, 0.9f), 0.2f);
		}
		else if (event.action == ds::AT_MOVE_BY) {
			float ttl = math::random(_settings->wanderer.minRotationTTL, _settings->wanderer.maxRotationTTL);
			_world->rotateBy(event.id, getRotationAngle(), ttl);
			_world->scale(event.id, v3(0.9f, 0.9f, 0.9f), v3(1.0f, 1.0f, 1.0f), 0.2f);
		}
	}
};
