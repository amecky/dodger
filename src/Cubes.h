#pragma once
#include <core\io\DataFile.h>
#include <core\io\json.h>
#include <core\lib\collection_types.h>
#include <renderer\sprites.h>
#include "GameContext.h"
#include <core\lib\DataArray.h>
#include "BallEmitter.h"

// ---------------------------------------
// Cube definition
// ---------------------------------------
struct CubeDefinition {

	char name[20];
	float velocity;
	float velocityVariance;
	int numStars;
	int behaviorBits;
	float growTTL;
	int type;
};

// ---------------------------------------
// Cube definitions
// ---------------------------------------
class CubeDefinitions : public ds::JSONAssetFile {

public:
	CubeDefinitions() : ds::JSONAssetFile("resources\\cube_definitions.json") {}
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

// ---------------------------------------
// Wave definition
// ---------------------------------------
struct WaveDefinition {
	int cubeType;
	int maxConcurrent;
	int numSpawn;
	float spawnTTL;	
};

// ---------------------------------------
// Wave definitions
// ---------------------------------------
class WaveDefinitions : public ds::JSONAssetFile {

public:
	WaveDefinitions() : ds::JSONAssetFile("resources\\wave_definitions.json") {}
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
struct Ball {

	v2 velocity;
	v2 force;
	int def_index;
	int wave_index;

	Ball() : velocity(0, 0), force(0, 0) , def_index(0) , wave_index(0) {}

};

// ---------------------------------------
// Wave runtime
// ---------------------------------------
struct WaveRuntime {
	int definitionIndex;
	int current;
	int total;
	float timer;
};

typedef ds::Array<WaveRuntime> WaveRuntimes;

// ---------------------------------------
// Cubes
// ---------------------------------------
class Cubes {

public:
	Cubes(GameContext* context);
	~Cubes();
	void activate();
	void spawn(float dt);
	int killBalls(const v2& bombPos, KilledBall* positions);
	void killAll(bool explode = true);
	void emitt(int type);
	void handleEvents(const ds::ActionEventBuffer& buffer);
	void move(float dt);
private:	
	void createBall(const v2& pos, int current, int total, int waveDefinitionIndex);
	void seek(const v2& target, float velocity);
	void separate(const v2& target, float minDistance, float relaxation);
	void align(const v2& target, float desiredDistance);
	GameContext* _context;
	ds::World* _world;
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
