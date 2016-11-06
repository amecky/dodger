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

enum StageMode {
	SM_WARMING_UP,
	SM_RUNNING,
	SM_DONE
};

struct ActiveStageEntry {
	float timer;
	StageMode mode;
	int counter;
	float emittDelay;
	StageEntry entry;
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
	void startStage(int index);
	int getTotalCount() const {
		return _total;
	}
private:
	ds::Array<Stage> _stages;
	ds::Array<ActiveStageEntry> _activeStages;
	int _selectedStage;
	float _timer;
	int _total;
};

