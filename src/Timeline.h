#pragma once
#include <core\lib\collection_types.h>
#include <core\string\StaticHash.h>

struct EmitterEvent {
	v2 pos;
	StaticHash type;
};

typedef ds::Stack<EmitterEvent> EmitterQueue;

enum TimelineEntryState {
	TLES_WAITING,
	TLES_ACTIVE,
	TLES_DONE
};

struct TimelineEntry {
	int ticks;
	StaticHash type;
	int num;
	int emitter;
	float delay;
	TimelineEntryState state;
	float timer;
	int emitted;
};

class Timeline {

public:
	Timeline();
	~Timeline();
	void start();
	void load();
	void tick(EmitterQueue& queue,float dt);
private:
	v2 getPosition(const TimelineEntry& entry);
	int _ticks;
	ds::Array<TimelineEntry> _entries;
};

