#pragma once
#pragma once
#include "..\GameContext.h"
#include <core\base\GameObject.h>
#include "FourierPathContainer.h"

class Stages : public ds::GameObject {

	enum StageState {
		PREPARING,
		WAITING,
		RUNNING,
		IDLE,
		EOL
	};

	struct StageObject {
		ID id;
		float timer;
		float y;
		float amplitude;
		int path;
	};

	typedef ds::Array<StageObject> StageObjects;

public:
	Stages(GameContext* ctx);
	~Stages();
	void tick(float dt);
	void onActivation();
	void onDeactivation();	
	bool killEnemy(const ds::Collision& c, int objectType);
private:
	void emittEnemy(float ypos);
	void wait(float dt, float ttl, StageState next);
	void moveObjects(float dt);
	GameContext* _context;
	StageObjects _objects;
	FourierPathContainer _pathContainer;
	StageState _state;
	float _timer;
	int _emitted;
	int _currentPath;
};


