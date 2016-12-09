#pragma once
#include <core\lib\collection_types.h>
#include <core\string\StaticHash.h>
#include "PointEmitter.h"

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
	PointEmitter* emitter;
	float delay;
	TimelineEntryState state;
	float timer;
	int emitted;
	p2i grid;
};

class Timeline {

public:
	Timeline();
	~Timeline();
	void start();
	void load();
	void tick(EmitterQueue& queue,float dt);
private:
	int _ticks;
	ds::Array<TimelineEntry> _entries;
};

