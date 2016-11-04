#include "Stages.h"


Stages::Stages() : ds::JSONAssetFile("content\\resources\\asteroid_levels.json") {
}


Stages::~Stages() {
}


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

bool Stages::reloadData(const ds::JSONReader& loader) {
	_stages.clear();
	return loadData(loader);
}

int Stages::tick(float dt, int* types, int max) {

}