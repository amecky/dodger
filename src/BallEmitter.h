#pragma once
#include <Vector.h>
#include <math\GameMath.h>
#include <renderer\graphics.h>

// --------------------------------------
// spawn point
// --------------------------------------
struct SpawnPoint {

	v2 position;
	v2 normal;

};

// --------------------------------------
// spawner type
// --------------------------------------
enum SpawnerType {
	SPT_EDGES,
	SPT_PARTIAL_EDGES,
	SPT_EOL
};

// --------------------------------------
// start point
// --------------------------------------
struct StartPoint {
	v2 position;
	float timer;
	v2 normal;
};

// --------------------------------------
// start points
// --------------------------------------
typedef std::vector<StartPoint> StartPoints;

// --------------------------------------
// spawner emitter type
// --------------------------------------
enum SpawnEmitterType {
	SET_DELAYED,
	SET_IMMEDIATE,
	SET_EOL
};

// --------------------------------------
// spawner data
// --------------------------------------
struct SpawnerData {
	int count_x;
	int count_y;
	v2 border;
	v2 random_offset;
	int sides;
	SpawnerType type;
	SpawnEmitterType emitter_type;
	float delay;

	SpawnerData() : count_x(0), count_y(0), border(0, 0), random_offset(0, 0), sides(0), type(SPT_EOL), emitter_type(SET_DELAYED), delay(0.0f) {}
};
// --------------------------------------
// enemy spawner
// --------------------------------------
class BallEmitter {

public:
	BallEmitter(const SpawnerData& data);
	virtual ~BallEmitter() {
		if (_points != 0) {
			delete[] _points;
		}
	}
	void rebuild();
	const SpawnPoint& next() {
		if ((_index + 1) > _total) {
			_index = 0;
		}
		return _points[_index++];
	}

	const SpawnPoint& random() {
		int o = ds::math::random(0, _total);
		return _points[o];
	}
	const int size() const {
		return _total;
	}
	void from_grid(int x, int y, const v2& steps, v2& p) {
		p.x = _data.border.x + static_cast<float>(x)* steps.x;
		p.y = _data.border.y + static_cast<float>(y)* steps.y;
	}
protected:
	int _index;
	SpawnPoint* _points;
	int _total;
	SpawnerData _data;
};
