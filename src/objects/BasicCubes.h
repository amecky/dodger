#pragma once
#include <core\world\World.h>
#include "..\GameSettings.h"
#include "..\Constants.h"

class CubeEmitter {

public:
	virtual void next() = 0;
	const v2& get() const {
		return _position;
	}
	virtual v2 get(int index, int total) const = 0;
protected:
	v2 _position;
};

class TestCubeEmitter : public CubeEmitter {

public:
	void next() {
		_position = v2(512, 384);
	}

	v2 get(int index, int total) const {
		return _position;
	}
};

class RandomCubeEmitter : public CubeEmitter {

public:

	void next() {
		float x = math::random(150.0f, 900.0f);
		float y = math::random(150.0f, 600.0f);
		_position = v2(x, y);
	}

	v2 get(int index, int total) const {
		return _position;
	}
};

class CircleCubeEmitter : public CubeEmitter {

public:
	CircleCubeEmitter(float radius) : _radius(radius) {}

	void next() {
		float x = math::random(150.0f, 900.0f);
		float y = math::random(150.0f, 600.0f);
		_position = v2(x, y);
	}

	v2 get(int index, int total) const {
		float step = TWO_PI / static_cast<float>(total);
		float angle = step * static_cast<float>(index);
		return _position + math::getRadialVelocity(angle, _radius);
	}

private:
	float _radius;
};

struct SpawnSettings {
	float minDelay;
	float maxDelay;
	int maxCubes;
	int emittCubes;
};

class BasicCubes {

public:
	BasicCubes(ds::World* world, CubeEmitter* emitter, GameSettings* settings);
	~BasicCubes();
	void reset();
	void init(const SpawnSettings& spawnSettings);
	void tick(ID target, float dt);
	virtual void create() = 0;
	virtual ObjectType getObjectType() const = 0;
	virtual void onEvent(const ds::ActionEvent& event, ID target, float dt) = 0;
protected:
	ds::World* _world;
	GameSettings* _settings;
	float _timer;
	float _spawnDelay;
	int _emitted;
	bool _running;
	SpawnSettings _spawnSettings;
	CubeEmitter* _emitter;
};

