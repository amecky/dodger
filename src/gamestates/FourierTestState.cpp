#include "FourierTestState.h"
#include <renderer\sprites.h>
#include "..\Constants.h"
#include <Vector.h>
#include <core\math\matrix.h>
#include <core\log\Log.h>
#include "base\InputSystem.h"
#include <core\math\GameMath.h>
#include <core\io\ReportWriter.h>
#include "..\objects\WarpingGrid.h"
#include "..\objects\ElasticBorder.h"

FourierTestState::FourierTestState(GameContext* context) : ds::GameState("FourierTestState"), _context(context) {
	
	_font = ds::res::find("nullshock", ds::ResourceType::BITMAPFONT);
	_bullets = (Bullets*)ds::game::get_game_object(SID("Bullets"));
	_stages = (Stages*)ds::game::get_game_object(SID("Stages"));
	_hud = ds::res::getGUIDialog("HUD");
	_hud->setNumber(2, 0);
	_dialogState = 1;
	_dialogPos = v2(10, 760);

	_drawDebug = true;
}


FourierTestState::~FourierTestState() {
}

// -------------------------------------------------------
// init
// -------------------------------------------------------
void FourierTestState::init() {
	
}

// -------------------------------------------------------
// activate
// -------------------------------------------------------
void FourierTestState::activate() {
	ds::game::activate_game_object(SID("Player"));
	ds::game::activate_game_object(SID("Bullets"));
	//ds::game::activate_game_object(SID("Stages"));
	_bullets->stop();	
	_hud->activate();
}

// -------------------------------------------------------
// dactivate
// -------------------------------------------------------
void FourierTestState::deactivate() {
	killPlayer();
	_hud->deactivate();
}

void FourierTestState::killPlayer() {
	_bullets->killAll();
	ds::game::deactivate_game_object(SID("Player"));
	ds::game::deactivate_game_object(SID("Bullets"));
	ds::game::deactivate_game_object(SID("Stages"));
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
int FourierTestState::update(float dt) {
	ZoneTracker u2("MainGameState::update");

	_context->world->tick(dt);

	{
		ZoneTracker u2("MainGameState::events");
		uint32_t n = ds::events::num();
		if (n > 0) {
			for (uint32_t i = 0; i < n; ++i) {
				if (ds::events::getType(i) == 100) {
					LOG << "writing report now";
					ds::ReportWriter writer("reports\\world.html");
					_context->world->saveReport(writer);
				}
			}
		}
	}
	if (handleCollisions(dt)) {
		return 1;
	}
	{
		ZoneTracker u2("MainGameState::worldEvents");
		if (_context->world->hasEvents()) {
			uint32_t n = _context->world->numEvents();
			for (uint32_t i = 0; i < n; ++i) {
				const ds::ActionEvent& event = _context->world->getEvent(i);
				if (event.action == ds::AT_MOVE_BY && event.type == OT_BULLET) {
					v3 bp = _context->world->getPosition(event.id);
					_context->elasticBorder->splash(bp, -30);
					_bullets->kill(event.id);
				}
			}
		}
	}

	_hud->tick(dt);
	
	return 0;
}

// -------------------------------------------------------
// handle collisions
// -------------------------------------------------------
bool FourierTestState::handleCollisions(float dt) {
	ZoneTracker u2("MainGameState::handleCollisions");
	if (_context->world->hasCollisions()) {
		uint32_t n = _context->world->numCollisions();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::Collision& c = _context->world->getCollision(i);
			if (c.isBetween(OT_PLAYER, OT_FOLLOWER)) {
				//killEnemy(c, OT_FOLLOWER);				
				//return true;
			}
			else if (c.isBetween(OT_BULLET, OT_FOLLOWER)) {
				killEnemy(c, OT_FOLLOWER);				
			}
			else if (c.isBetween(OT_BULLET, OT_WANDERER)) {
				killEnemy(c, OT_WANDERER);
			}
			else if (c.isBetween(OT_BULLET, OT_SPOTTER)) {
				killEnemy(c, OT_SPOTTER);				
			}
		}
	}
	return false;
}

// -------------------------------------------------------
// kill enemy
// -------------------------------------------------------
bool FourierTestState::killEnemy(const ds::Collision& c, int objectType) {
	bool ret = false;
	_stages->killEnemy(c, objectType);	
	if (c.containsType(OT_BULLET)) {
		_bullets->kill(c.getIDByType(OT_BULLET));
	}
	return ret;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void FourierTestState::render() {
	//ds::Path* path = _pathContainer.get(_pathIndex);
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	sprites->begin();
	/*
	if (_drawDebug) {
		v2 pos;		
		for (int i = 0; i < 128; ++i) {
			float t = static_cast<float>(i) / 128.0f;
			path->approx(t, &pos);
			pos.y = 360.0f + pos.y;
			sprites->draw(pos, math::buildTexture(60, 120, 8, 8));
		}
	}
	*/
	v2 d = sprites->getTextSize(_font, "Stage 1");
	int dx = (1024 - d.x) / 2;
	sprites->drawText(_font, dx, 388, "Stage 1", 4, 1.0f, 1.0f, ds::Color(192, 0, 0, 255));
	d = sprites->getTextSize(_font, "Open space");
	dx = (1024 - d.x) / 2;
	sprites->drawText(_font, dx, 348, "Open space", 4, 1.0f, 1.0f, ds::Color(192, 0, 0, 255));
	_hud->render();
	sprites->end();
	/*
	gui::start(1, &_dialogPos);
	gui::begin("Item", &_dialogState);
	gui::InputInt("Idx", &_pathIndex,0,_pathContainer.num()-1,1);
	/*
	gui::InputInt("Num", &path.num,0,7,1);
	gui::InputFloat("1.", &path.values[0]);
	gui::InputFloat("2.", &path.values[1]);
	gui::InputFloat("3.", &path.values[2]);
	gui::InputFloat("4.", &path.values[3]);
	gui::InputFloat("5.", &path.values[4]);
	gui::InputFloat("6.", &path.values[5]);
	gui::InputFloat("7.", &path.values[6]);
	gui::InputFloat("8.", &path.values[7]);
	gui::InputFloat("TTL", &path.ttl);
	gui::InputFloat("Offset", &path.offset);
	gui::InputFloat("Height", &path.height);
	gui::InputFloat("Frequency", &path.frequency);
	if (gui::Button("Build")) {
		path->build();
	}
	if (gui::Button("Start")) {
		path->build();
		_emitting = true;
		_emitterTimer = 0.0f;
		_emitted = 0;
	}
	if (gui::Button("Start One")) {
		path->build();
		emittEnemy(360.0f);
	}
	if (gui::Button("Toggle debug")) {
		_drawDebug = !_drawDebug;
	}
	gui::end();
	*/
}


// -------------------------------------------------------
// on char
// -------------------------------------------------------
int FourierTestState::onChar(int ascii) {
	if (ascii == 'e') {
		return 1;
	}
	if (ascii == 'p') {
		return 2;
	}
	if (ascii == 'c') {
		for (int i = 0; i < 5; ++i) {
			float x = math::random(100.0f, 900.0f);
			float y = math::random(100.0f, 620.0f);
			//_context->particles->start(1, v2(x, y));
			_context->particles->start(5, v2(x, y));
			_context->particles->start(6, v2(x, y));
		}
	}
	return 0;
}

