#pragma once
#include <Vector.h>

class DebugPanel {

struct DebugEntry {
	
	enum DataType {
		DT_FLOAT,
		DT_VEC2,
		DT_VEC3,
		DT_INT,
		DT_COLOR,
		DT_EOL
	};

	const char* name;
	float values[4];
	DataType type;
	
};

public:
	DebugPanel();
	~DebugPanel();
	void render();
	void reset();
	void show(const char* name, float value);
	void show(const char* name, const v2& value);
	void show(const char* name, int value);
private:
	DebugEntry _entries[32];
	int _count;
	v2 _pos;
	int _state;
};

