#include "Timeline.h"
#include <core\io\json.h>

Timeline::Timeline() : _ticks(0) {
}


Timeline::~Timeline() {
	for (uint32_t i = 0; i < _entries.size(); ++i) {
		delete _entries[i].emitter;
	}
}

// -------------------------------------------------------
// start
// -------------------------------------------------------
void Timeline::start() {
	_ticks = 0;
	for (uint32_t i = 0; i < _entries.size(); ++i) {
		_entries[i].state = TLES_WAITING;
		_entries[i].timer = 0.0f;
		_entries[i].emitted = 0;
	}
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
void Timeline::tick(EmitterQueue& queue, float dt) {
	++_ticks;
	for (uint32_t i = 0; i < _entries.size(); ++i) {
		TimelineEntry& entry = _entries[i];
		if (entry.state == TLES_ACTIVE) {
			entry.timer += dt;
			if (entry.timer >= entry.delay) {
				entry.timer = 0.0f;				
				EmitterEvent ed;
				ed.pos = entry.emitter->next(entry.emitted);
				ed.type = entry.type;
				ed.behavior = entry.behavior;
				queue.push(ed);
				++entry.emitted;
				if (entry.emitted == entry.num) {
					entry.state = TLES_DONE;
				}
			}
		}
		else if (entry.state == TLES_WAITING) {
			if (_ticks >= entry.ticks) {
				entry.emitted = 1;
				EmitterEvent ed;
				ed.pos = entry.emitter->next(0);
				ed.type = entry.type;
				ed.behavior = entry.behavior;
				queue.push(ed);
				if (entry.num == 1) {					
					entry.state = TLES_DONE;
				}
				else {
					entry.state = TLES_ACTIVE;
				}
			}
		}
	}
}

// -------------------------------------------------------
// load
// -------------------------------------------------------
void Timeline::load() {
	ds::JSONReader reader;
	if (reader.parse(SID("content\\resources\\enemies.json"))) {
		int items[256];
		int num = reader.get_categories(items, 256);
		for (int i = 0; i < num; ++i) {
			TimelineEntry entry;
			reader.get_int(items[i], "ticks", &entry.ticks);
			reader.get(items[i], "type", &entry.type);
			reader.get_int(items[i], "num", &entry.num);
			reader.get(items[i], "grid", &entry.grid);
			int emitter = 0;
			reader.get_int(items[i], "emitter", &emitter);
			switch (emitter) {
				case 1: entry.emitter = new RandomPointEmitter(entry.num, entry.grid); break;
				case 2: entry.emitter = new VerticalLineEmitter(entry.num, entry.grid); break;
				case 3: entry.emitter = new HorizontalLineEmitter(entry.num, entry.grid); break;
				default: entry.emitter = new RandomPointEmitter(entry.num, entry.grid);
			}
			reader.get(items[i], "delay", &entry.delay);		
			reader.get(items[i], "behavior", &entry.behavior);
			entry.state = TLES_WAITING;
			entry.timer = 0.0f;
			entry.emitted = 0;
			_entries.push_back(entry);
		}
	}
}