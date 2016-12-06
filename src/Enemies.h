#pragma once
#include <core\lib\collection_types.h>

struct EmitterEvent {
	v2 pos;
	int type;
};

typedef ds::Stack<EmitterEvent> EmitterQueue;

enum TimelineEntryState {
	TLES_WAITING,
	TLES_ACTIVE,
	TLES_DONE
};

struct TimelineEntry {
	int ticks;
	int type;
	int num;
	int emitter;
	float delay;
	TimelineEntryState state;
	float timer;
	int emitted;
};

class Enemies {

public:
	Enemies();
	~Enemies();
	void start();
	void load();
	void tick(EmitterQueue& queue,float dt);
private:
	v2 getPosition(const TimelineEntry& entry);
	int _ticks;
	ds::Array<TimelineEntry> _entries;
};

