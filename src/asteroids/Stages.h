#pragma once
#include <core\io\DataFile.h>

struct StageEntry {
	int type;
	float initialDelay;
	float delay;
	float delayVariance;
	int total;
	int emitt;
};

struct Stage {
	ds::Array<StageEntry> entries;
};

class Stages : public ds::JSONAssetFile {

public:
	Stages();
	~Stages();
	bool loadData(const ds::JSONReader& loader);
	bool reloadData(const ds::JSONReader& loader);
	int tick(float dt, int* types, int max);
private:
	ds::Array<Stage> _stages;
};

