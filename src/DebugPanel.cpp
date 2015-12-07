#include "DebugPanel.h"
#include <ui\IMGUI.h>

DebugPanel::DebugPanel() : _count(0) , _pos(10,710), _state(1) { 
}


DebugPanel::~DebugPanel() {
}

void DebugPanel::reset() {
	_count = 0;
}

void DebugPanel::show(const char* name, float value) {
	if (_count + 1 < 32) {
		DebugEntry& entry = _entries[_count++];
		entry.name = name;
		entry.type = DebugEntry::DT_FLOAT;
		entry.values[0] = value;
	}
}

void DebugPanel::show(const char* name, int value) {
	if (_count + 1 < 32) {
		DebugEntry& entry = _entries[_count++];
		entry.name = name;
		entry.type = DebugEntry::DT_INT;
		entry.values[0] = value;
	}
}

void DebugPanel::show(const char* name, const v2& value) {
	if (_count + 1 < 32) {
		DebugEntry& entry = _entries[_count++];
		entry.name = name;
		entry.type = DebugEntry::DT_VEC2;
		entry.values[0] = value.x;
		entry.values[1] = value.y;
	}
}

void DebugPanel::render() {
	gui::start(1, &_pos);
	char buffer[128];
	if (gui::begin("Debug values", &_state)) {
		for (int i = 0; i < _count; ++i) {
			const DebugEntry& entry = _entries[i];
			switch (entry.type) {
				case DebugEntry::DT_FLOAT : sprintf_s(buffer, 128, "%s : %3.2f", entry.name, entry.values[0]); break;
				case DebugEntry::DT_VEC2: sprintf_s(buffer, 128, "%s : %3.2f , %3.2f", entry.name, entry.values[0], entry.values[1]); break;
				case DebugEntry::DT_INT: sprintf_s(buffer, 128, "%s : %d", entry.name, static_cast<int>(entry.values[0])); break;
			}
			gui::Label(2 + i, buffer);
		}
	}
	gui::end();
}