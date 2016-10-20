#include "Levels.h"
#include <core\io\json.h>

const StaticHash NAMES[] = { SID("Follower"), SID("Wanderer"), SID("Spotter") };

Levels::Levels() {
}


Levels::~Levels() {
}

void Levels::load() {
	ds::JSONReader reader;
	if (reader.parse(SID("content\\resources\\levels.json"))) {
		int stages[64];
		int num = reader.get_categories(stages, 64);
		for (int i = 0; i < num; ++i) {
			int id = -1;
			if (reader.get_int(stages[i], "id", &id)) {
				int cubes[32];
				int nc = reader.get_categories(cubes, 32, stages[i]);
				Level& level = _levels[id];
				level.num = 0;
				for (int j = 0; j < nc; ++j) {
					LevelItem& item = level.items[j];
					reader.get_int(cubes[j], "id", &item.cubeType);
					reader.get_int(cubes[j], "emitt", &item.emitt);
					reader.get_int(cubes[j], "emitter", &item.emitter);
					reader.get_int(cubes[j], "max", &item.num);
					reader.get(cubes[j], "delay", &item.delay);
					++level.num;
				}
			}
		}
	}
}

const Level& Levels::get(int index) const {
	return _levels[index];
}

int Levels::getNumberOfItems(int index) const {
	const Level& l = _levels[index];
	return l.num;
}

void Levels::start(int index) {

}
StageActor* Levels::create(int level, int offset) {
	StageActor* actor = new StageActor();
	const Level& l = _levels[level];
	const LevelItem& li = l.items[offset];
	actor->mode = SAM_STARTED;
	actor->delay = li.delay;
	actor->emitted = 0;
	actor->num = li.emitt;
	actor->total = li.num;
	actor->templateName = NAMES[li.cubeType];
	return actor;
}