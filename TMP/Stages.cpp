#include "Stages.h"


Stages::Stages() : ds::JSONAssetFile("content\\resources\\asteroid_levels.json") {
	_selectedStage = -1;
	_timer = 0.0f;
}


Stages::~Stages() {
}

// -------------------------------------------------------
// load data
// -------------------------------------------------------
bool Stages::loadData(const ds::JSONReader& loader) {
	int categories[16];
	int sub[16];
	int num = loader.get_categories(categories, 16);
	for (int i = 0; i < num; ++i) {
		Stage stage;
		int n = loader.get_categories(sub, 16, categories[i]);
		for (int j = 0; j < n; ++j) {
			StageEntry entry;
			loader.get_int(sub[i], "type", &entry.type);
			loader.get(sub[i], "initial_delay", &entry.initialDelay);
			loader.get(sub[i], "delay", &entry.delay);
			loader.get(sub[i], "delay_variance", &entry.delayVariance);
			loader.get_int(sub[i], "total", &entry.total);
			loader.get_int(sub[i], "emitt", &entry.emitt);
			stage.entries.push_back(entry);
		}
		_stages.push_back(stage);
	}
	return true;
}

// -------------------------------------------------------
// realod data
// -------------------------------------------------------
bool Stages::reloadData(const ds::JSONReader& loader) {
	_stages.clear();
	return loadData(loader);
}

// -------------------------------------------------------
// clear
// -------------------------------------------------------
int Stages::getActiveTypes(int* types, int max) {
	int cnt = 0;
	if (!_activeStages.empty()) {
		for (uint32_t i = 0; i < _activeStages.size(); ++i) {
			ActiveStageEntry& ase = _activeStages[i];
			if (cnt < max) {
				types[cnt++] = ase.entry.type;
			}
		}
	}
	return cnt;
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
int Stages::tick(float dt, int* types, int max) {
	int cnt = 0;
	if (!_activeStages.empty()) {
		_timer += dt;
		for (uint32_t i = 0; i < _activeStages.size(); ++i) {
			ActiveStageEntry& ase = _activeStages[i];
			if (ase.mode == SM_WARMING_UP) {
				if (ase.timer >= ase.entry.initialDelay) {
					ase.timer = 0.0f;
					ase.mode = SM_RUNNING;
					ase.emittDelay = 0.0f;// ase.entry.delay + 20.0f;// math::randomRange(ase.entry.delay, ase.entry.delayVariance);
				}
			}
			else if (ase.mode == SM_RUNNING) {
				if (ase.timer >= ase.emittDelay) {
					int count = ase.entry.emitt;
					for (int j = 0; j < count; ++j) {
						if (cnt < max) {
							types[cnt++] = ase.entry.type;
							++ase.counter;
						}
					}
					ase.timer = 0.0f;
					ase.emittDelay = math::randomRange(ase.entry.delay, ase.entry.delayVariance);
					if (ase.counter >= ase.entry.total) {
						ase.mode = SM_DONE;
					}
				}
			}
			ase.timer += dt;
		}
	}
	return cnt;
}

// -------------------------------------------------------
// start stage
// -------------------------------------------------------
void Stages::startStage(int index) {
	_timer = 0.0f;
	_selectedStage = index;
	_activeStages.clear();
	_total = 0;
	const Stage& stage = _stages[index];
	for (uint32_t i = 0; i < stage.entries.size(); ++i) {
		const StageEntry& se = stage.entries[i];
		ActiveStageEntry ase;
		ase.counter = 0;
		ase.mode = SM_WARMING_UP;
		ase.entry = se;
		ase.timer = 0.0f;
		_activeStages.push_back(ase);
		_total += (1 + std::pow(2,3 -se.type)) * se.total;
	}
}
