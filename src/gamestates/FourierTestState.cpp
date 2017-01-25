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
	
	_bullets = (Bullets*)ds::game::get_game_object(SID("Bullets"));
	_hud = ds::res::getGUIDialog("HUD");
	_hud->setNumber(2, 0);
	_dialogState = 1;
	_dialogPos = v2(10, 760);

	_pathIndex = 0;
	_pathContainer.load();

	_drawDebug = true;

	_emitting = false;
	_emitterTimer = 0.0f;
	_emitted = 0;

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
	//_context->player->activate();
	ds::game::activate_game_object(SID("Player"));
	ds::game::activate_game_object(SID("Bullets"));
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
	//_context->player->deactivate();
	ds::game::deactivate_game_object(SID("Player"));
	ds::game::deactivate_game_object(SID("Bullets"));
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
int FourierTestState::update(float dt) {
	ZoneTracker u2("MainGameState::update");

	//_context->player->tick(dt);

	const ds::Path* path = _pathContainer.get(_pathIndex);
	Objects::iterator it = _objects.begin();
	while (it != _objects.end()) {
		it->timer += dt;
		float t = it->timer / _pathContainer.getTTL(_pathIndex);
		v2 pp;
		path->approx(t, &pp);
		v3 p = _context->world->getPosition(it->id);
		pp.y = it->y + pp.y;
		if (pp.x < 10.0f) {
			_context->world->remove(it->id);
			it = _objects.remove(it);
		}
		else {
			float angle = math::getAngle(p.xy(),pp);
			_context->world->setPosition(it->id, pp);
			_context->world->setRotation(it->id, angle);
			++it;
		}
	}

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
	//_bullets->tick(dt);

	_hud->tick(dt);
	/*
	if (_levelRunning && _levels->isActive() && (_kills == _levels->getNumberToKill())) {
		LOG << "ALL KILLED - NEXT LEVEL!!!";
		_levelRunning = false;
	}
	*/

	if (_emitting) {
		_emitterTimer += dt;
		if (_emitterTimer > 0.3f) {
			emittEnemy(384.0f);
			_emitterTimer = 0.0f;
			++_emitted;
			if (_emitted >= 10) {
				_emitting = false;
			}
		}
	}
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
	ID id = c.getIDByType(objectType);
	if (_context->world->contains(id)) {
		Objects::iterator it = _objects.begin();
		while (it != _objects.end()) {
			if ( id == it->id) {
				v3 p = _context->world->getPosition(id);
				_context->grid->applyForce(p.xy(), 0.3f, 5.0f, 40.0f);
				_context->world->remove(it->id);
				it = _objects.remove(it);
			}
			else {
				++it;
			}
		}
	}
	if (c.containsType(OT_BULLET)) {
		_bullets->kill(c.getIDByType(OT_BULLET));
	}
	return ret;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void FourierTestState::render() {
	ds::Path* path = _pathContainer.get(_pathIndex);
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	sprites->begin();
	if (_drawDebug) {
		v2 pos;		
		for (int i = 0; i < 128; ++i) {
			float t = static_cast<float>(i) / 128.0f;
			path->approx(t, &pos);
			pos.y = 360.0f + pos.y;
			sprites->draw(pos, math::buildTexture(60, 120, 8, 8));
		}
	}
	_hud->render();
	sprites->end();

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
	*/
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
}

void FourierTestState::emittEnemy(float ypos) {
	FObject o;
	o.timer = 0.0f;
	o.y = ypos;
	o.amplitude = math::random(70.0f, 110.0f);
	o.id = _context->world->create(v2(1280.0f, o.y), SID("Follower"));
	_context->world->attachCollider(o.id, ds::PST_CIRCLE);
	_objects.push_back(o);
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

