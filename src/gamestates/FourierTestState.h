#pragma once
#include <gamestates\GameState.h>
#include <renderer\render_types.h>
#include <imgui\IMGUI.h>
#include "..\GameContext.h"
#include <core\lib\DataArray.h>
#include <core\world\World.h>
#include <particles\ParticleManager.h>
#include "..\objects\Bullets.h"
#include "..\Enemies.h"

class FourierTestState : public ds::GameState {

	struct FObject {
		ID id;
		float timer;
		float y;
	};

	typedef ds::Array<FObject> Objects;

public:
	FourierTestState(GameContext* context);
	virtual ~FourierTestState();
	void init();
	int update(float dt);
	void render();
	void activate();
	void deactivate();
	int onChar(int ascii);
	int onButtonUp(int button, int x, int y);
	int onButtonDown(int button, int x, int y);
private:
	void killPlayer();
	void movePlayer(float dt);
	bool handleCollisions(float dt);
	bool killEnemy(const ds::Collision& c, int objectType);

	GameContext* _context;
	ID _player;
	ID _playerRing;
	v2 _playerPrevious;
	float _playerAngle;
	ID _cursor;
	ds::GUIDialog* _hud;
	Bullets* _bullets;
	v2 _cursor_pos;
	v2 _dialogPos;
	int _dialogState;

	ID _enemy;
	float _timer;
	float _values[5];
	int _num;
	float _xvel;
	float _speed;
	Objects _objects;
};

