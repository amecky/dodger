#pragma once
#include "..\GameContext.h"
#include <core\base\GameObject.h>

class SimplePlayer : public ds::GameObject {

public:
	SimplePlayer(GameContext* ctx);
	~SimplePlayer();
	void tick(float dt);
	void onActivation();
	void onDeactivation();
	const ID getID() const {
		return _id;
	}
	void setVelocity(float v) {
		_velocity = v;
	}
private:
	GameContext* _context;
	ID _id;
	v2 _previous;
	float _velocity;
};

