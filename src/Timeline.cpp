#include "Timeline.h"
#include <core\io\json.h>

Timeline::Timeline() : _ticks(0) {
}


Timeline::~Timeline() {
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
				++entry.emitted;
				EmitterEvent ed;
				ed.pos = getPosition(entry);
				ed.type = entry.type;
				queue.push(ed);
				if (entry.emitted == entry.num) {
					entry.state = TLES_DONE;
				}
			}
		}
		else if (entry.state == TLES_WAITING) {
			if (_ticks >= entry.ticks) {
				entry.emitted = 1;
				EmitterEvent ed;
				ed.pos = getPosition(entry);
				ed.type = entry.type;
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
// get position
// -------------------------------------------------------
v2 Timeline::getPosition(const TimelineEntry& entry) {
	if (entry.emitter == 1) {
		int sx = math::random(1, 40);
		int sy = math::random(1, 20);
		return v2(49 + sx * 30, 45 + sy * 30);
	}
	return v2(640, 360);
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
			reader.get_int(items[i], "emitter", &entry.emitter);
			reader.get(items[i], "delay", &entry.delay);
			entry.state = TLES_WAITING;
			entry.timer = 0.0f;
			entry.emitted = 0;
			_entries.push_back(entry);
		}
	}
}