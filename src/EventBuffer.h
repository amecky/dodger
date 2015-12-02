#pragma once
#include <Vector.h>

struct GameEvent {

	enum EventType {
		GE_BOMB_EXPLODED,
		GE_BOMB_ACTIVATED,
		GE_EOL
	};

	v2 position;
	EventType type;
};

struct EventBuffer {

	GameEvent events[64];
	int num;

	void add(GameEvent::EventType type, const v2& pos) {
		if (num + 1 < 64) {
			GameEvent& e = events[num++];
			e.type = type;
			e.position = pos;
		}
	}

	void add(const GameEvent& event) {
		if (num + 1 < 64) {
			events[num++] = event;
		}
	}

	void reset() {
		num = 0;
	}
};