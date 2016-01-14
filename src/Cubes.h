#pragma once
#include <io\DataFile.h>
#include <io\json.h>
#include <lib\collection_types.h>
#include <sprites\SpriteBatch.h>
#include "GameContext.h"
#include <lib\DataArray.h>
#include "BallEmitter.h"
#include "Stars.h"
#include "Behaviors.h"

struct CubeDefinition {

	char name[20];
	float velocity;
	float velocityVariance;
	int numStars;
	int behaviorBits;
	float growTTL;
	int type;
};

class CubeDefinitions : public ds::DataFile {

public:
	CubeDefinitions() {}
	~CubeDefinitions() {}
	bool saveData(ds::JSONWriter& writer) {
		return true;
	}
	bool loadData(const ds::JSONReader& reader);
	const char* getFileName() const {
		return "resources\\cube_definitions.json";
	}
	size_t size() const {
		return _definitions.size();
	}
	const CubeDefinition& get(size_t index) const {
		return _definitions[index];
	}
private:
	ds::Array<CubeDefinition> _definitions;
};

struct WaveDefinition {
	int cubeType;
	int maxConcurrent;
	int numSpawn;
	float spawnTTL;	
};

class WaveDefinitions : public ds::DataFile {

public:
	WaveDefinitions() {}
	~WaveDefinitions() {}
	bool saveData(ds::JSONWriter& writer) {
		return true;
	}
	bool loadData(const ds::JSONReader& reader);
	const char* getFileName() const {
		return "resources\\wave_definitions.json";
	}
	size_t size() const {
		return _definitions.size();
	}
	const WaveDefinition& get(size_t index) const {
		return _definitions[index];
	}
private:
	ds::Array<WaveDefinition> _definitions;
};

struct KilledBall {

	v2 position;
	int type;

};

// ----------------------------------------
// Ball
// ----------------------------------------
struct Ball : ds::BasicSprite {

	enum BallState {
		BS_GROWING,
		BS_STARTING,
		BS_MOVING,
		BS_EOL
	};
	v2 velocity;
	v2 force;
	float size;
	BallState state;
	int behaviors;
	float timer;
	int type;

	Ball() : ds::BasicSprite(), velocity(0, 0), force(0, 0), size(1.0f), state(BS_EOL), behaviors(0), timer(0.0f), type(0) {}

};

struct WaveRuntime {
	int definitionIndex;
	int current;
	int total;
	float timer;
};

typedef ds::Array<WaveRuntime> WaveRuntimes;

class Cubes {

public:
	Cubes(GameContext* context);
	~Cubes();
	void activate();
	void render();
	void tick(float dt);
	int killBalls(const v2& bombPos, KilledBall* positions);
	bool checkBallsInterception() const;
	void killAll();
	void emitt(int type);
private:
	void move(float dt);
	void createBall(const v2& pos, int current, int total, const CubeDefinition& cubeDefinition);
	void scaleGrowingBalls(float dt);
	void moveBalls(float dt);
	bool buildFromTemplate(Ball* ball, const char* name);

	GameContext* _context;
	ds::DataArray<Ball, MAX_BALLS> _balls;
	v2 _spawner_position;
	SpawnerData _spawnData;
	CubeDefinitions _cubeDefintions;
	WaveDefinitions _waveDefinitions;
	WaveRuntimes _waveRuntimes;
	StartPoints _startPoints;
	BallEmitter* _emitter;
	int _killed;
	int _emitted;
};
