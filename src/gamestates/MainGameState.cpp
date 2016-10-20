#include "MainGameState.h"
#include <renderer\sprites.h>
#include "..\Constants.h"
#include <Vector.h>
#include <core\math\matrix.h>
#include <core\log\Log.h>
#include "base\InputSystem.h"
#include <core\math\GameMath.h>
#include <core\io\ReportWriter.h>

MainGameState::MainGameState(GameContext* context) : ds::GameState("MainGame"), _context(context) {
	_world = new ds::World;
	
	//_world->setWorldDimension(v2(1600, 900));
	_world->setBoundingRect(ds::Rect(50, 50, 1500, 800));
	_world->useTemplates(ds::res::getWorldEntityTemplates(SID("game_objects")));
	_player = _world->create(v2(800, 450), math::buildTexture(40, 0, 40, 40), OT_PLAYER);
	_world->attachCollider(_player, ds::PST_CIRCLE, v2(40.f, 0.0));
	_playerAngle = 0.0f;
	_cursor = _world->create(v2(700, 384), math::buildTexture(40, 160, 20, 20), 100);

	_playerRing = _world->create(v2(100, 384), math::buildTexture(440, 0, 152, 152), OT_RING);

	_world->ignoreCollisions(OT_FOLLOWER, OT_FOLLOWER);
	_world->ignoreCollisions(OT_PLAYER, OT_BULLET);
	_world->ignoreCollisions(OT_BULLET, OT_BULLET);

	_dying = false;
	_dying_timer = 0.0f;

	ds::Viewport vw = ds::Viewport(1280, 720, 1600, 900);
	_viewport_id = graphics::addViewport(vw);

	_particles = ds::res::getParticleManager();

	_bullets = new Bullets(_world, context->settings);

	_border_color = ds::Color(181, 17, 68, 255);
	
	_levels = new Levels(_world, context->settings);
	_levels->load();

	_hud = ds::res::getGUIDialog("HUD");

	_level = 1;
	_hud->setNumber(2, _level);
}


MainGameState::~MainGameState() {
	delete _levels;
	delete _bullets;
}

// -------------------------------------------------------
// init
// -------------------------------------------------------
void MainGameState::init() {
	
}

// -------------------------------------------------------
// activate
// -------------------------------------------------------
void MainGameState::activate() {
	_bullets->stop();	
	_hud->activate();
}

// -------------------------------------------------------
// dactivate
// -------------------------------------------------------
void MainGameState::deactivate() {
	//_context->hudDialog->deactivate();
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int MainGameState::onButtonUp(int button, int x, int y) {
	_bullets->stop();
	return 0;
}

// -------------------------------------------------------
// on button up
// -------------------------------------------------------
int MainGameState::onButtonDown(int button, int x, int y) {
	_bullets->start(_player);
	return 0;
}

void MainGameState::killPlayer() {
	_dying = true;
	_dying_timer = 0.0f;
	_world->remove(_player);
	_world->remove(_playerRing);
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void MainGameState::movePlayer(float dt) {
	v2 vel = v2(0.0f);
	if (ds::input::getKeyState('A')) {
		vel.x -= 1.0f;
	}
	if (ds::input::getKeyState('D')) {
		vel.x += 1.0f;
	}
	if (ds::input::getKeyState('W')) {
		vel.y += 1.0f;
	}
	if (ds::input::getKeyState('S')) {
		vel.y -= 1.0f;
	}
	v2 cp = ds::input::getMousePosition();	
	const ds::Viewport& vp = graphics::getViewport(_viewport_id);
	v2 ncp = vp.convertToWorld(cp);
	_world->setPosition(_cursor, ncp);
	v2 wp = _world->getPosition(_player).xy();
	v2 pos = wp;
	float angle = 0.0f;
	ds::math::followRelative(ncp, wp, &_playerAngle, 5.0f, 1.1f * dt);
	_world->setRotation(_player, _playerAngle);
	pos += vel * 250.0f * dt;
	if (ds::math::isInside(pos, ds::Rect(0, 0, 1600, 900))) {			
		_world->setPosition(_player, pos);
		_world->setPosition(_playerRing, pos);
	}
	graphics::setViewportPosition(_viewport_id, pos);
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
int MainGameState::update(float dt) {
	
	movePlayer(dt);

	const v2 wp = _world->getPosition(_player).xy();
	moveStars(wp, dt);

	_world->tick(dt);

	uint32_t n = ds::events::num();
	if (n > 0) {
		for (uint32_t i = 0; i < n; ++i) {
			if (ds::events::getType(i) == 100) {
				LOG << "writing report now";
				ds::ReportWriter writer("reports\\world.html");
				_world->saveReport(writer);
			}
		}
	}
	
	handleCollisions(dt);

	if (_world->hasEvents()) {
		uint32_t n = _world->numEvents();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::ActionEvent& event = _world->getEvent(i);
			if (event.action == ds::AT_MOVE_BY && event.type == OT_BULLET) {
				_bullets->kill(event.id);
			}
		}
	}

	_bullets->tick(dt);

	_hud->tick(dt);

	_levels->tick(_player, dt);
	return 0;
}

// -------------------------------------------------------
// handle collisions
// -------------------------------------------------------
void MainGameState::handleCollisions(float dt) {
	if (_world->hasCollisions()) {
		uint32_t n = _world->numCollisions();
		for (uint32_t i = 0; i < n; ++i) {
			const ds::Collision& c = _world->getCollision(i);
			if (c.isBetween(OT_PLAYER, OT_STAR)) {
				ID id = c.getIDByType(OT_STAR);
				_world->remove(id);
			}
			else if (c.isBetween(OT_PLAYER, OT_FOLLOWER)) {
				ID id = c.getIDByType(OT_FOLLOWER);
				_world->remove(id);
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
}

// -------------------------------------------------------
// kill enemy
// -------------------------------------------------------
bool MainGameState::killEnemy(const ds::Collision& c, int objectType) {
	bool ret = false;
	ID id = c.getIDByType(objectType);
	if (_world->contains(id)) {
		CubeData* data = (CubeData*)_world->get_data(id);
		--data->energy;
		if (data->energy <= 0) {
			v3 p = _world->getPosition(id);
			_particles->start(5, p.xy());
			_particles->start(6, p.xy());
			_world->remove(id);
			ret = true;
		}
	}
	_bullets->kill(c.getIDByType(OT_BULLET));
	return ret;
}

// -------------------------------------------------------
// draw border
// -------------------------------------------------------
void MainGameState::drawBorder() {
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	// background
	sprites->draw(v2(480, 306), math::buildTexture(0, 512, 480, 306), 0.0f, v2(2.0f, 2.0f));
	sprites->draw(v2(1280, 306), math::buildTexture(0, 512, 320, 306), 0.0f, v2(2.0f, 2.0f));
	sprites->draw(v2(480, 756), math::buildTexture(0, 512, 480, 144), 0.0f, v2(2.0f, 2.0f));
	sprites->draw(v2(1280, 756), math::buildTexture(0, 512, 320, 144), 0.0f, v2(2.0f, 2.0f));
	// 4 corners
	sprites->draw(v2(40, 860), math::buildTexture(840, 0, 40, 60), 0.0f, v2(1.0f, 1.0f), _border_color);
	sprites->draw(v2(40, 40), math::buildTexture(940, 0, 40, 60), 0.0f, v2(1.0f, 1.0f), _border_color);
	sprites->draw(v2(1560, 860), math::buildTexture(840, 280, 40, 60), 0.0f, v2(1.0f, 1.0f), _border_color);
	sprites->draw(v2(1560, 40), math::buildTexture(940, 280, 40, 60), 0.0f, v2(1.0f, 1.0f), _border_color);
	// left and right wall
	for (int i = 0; i < 9; ++i) {
		sprites->draw(v2(40, 110 + i * 80), math::buildTexture(880, 0, 40, 80), 0.0f, v2(1.0f, 1.0f), _border_color);
		sprites->draw(v2(1560, 110 + i * 80), math::buildTexture(880, 280, 40, 80), 0.0f, v2(1.0f, 1.0f), _border_color);
	}
	// bottom and top wall
	for (int i = 0; i < 7; ++i) {
		sprites->draw(v2(160 + i * 200, 870), math::buildTexture(840, 40, 200, 40), 0.0f, v2(1.0f, 1.0f), _border_color);
		sprites->draw(v2(160 + i * 200, 30), math::buildTexture(960, 40, 200, 40), 0.0f, v2(1.0f, 1.0f), _border_color);
	}
	// missing left and right pieces
	sprites->draw(v2(40, 810), math::buildTexture(880, 0, 40, 40), 0.0f, v2(1.0f, 1.0f), _border_color);
	sprites->draw(v2(1560, 810), math::buildTexture(880, 280, 40, 40), 0.0f, v2(1.0f, 1.0f), _border_color);
	// missing top and bottom pieces
	sprites->draw(v2(1505, 870), math::buildTexture(840, 40, 90, 40), 0.0f, v2(1.0f, 1.0f), _border_color);
	sprites->draw(v2(1505, 30), math::buildTexture(960, 40, 90, 40), 0.0f, v2(1.0f, 1.0f), _border_color);
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void MainGameState::render() {	
	graphics::selectViewport(_viewport_id);
	drawBorder();
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	ds::ChannelArray* array = _world->getChannelArray();
	v3* positions = (v3*)array->get_ptr(ds::WEC_POSITION);
	ds::Texture* textures = (ds::Texture*)array->get_ptr(ds::WEC_TEXTURE);
	v3* rotations = (v3*)array->get_ptr(ds::WEC_ROTATION);
	v3* scales = (v3*)array->get_ptr(ds::WEC_SCALE);
	ds::Color* colors = (ds::Color*)array->get_ptr(ds::WEC_COLOR);
	for (uint32_t i = 0; i < array->size; ++i) {
		v2 p = positions[i].xy();
		float r = rotations[i].x;
		ds::Texture t = textures[i];
		sprites->draw(positions[i].xy(), textures[i], rotations[i].x,scales[i].xy(),colors[i]);
	}
	sprites->end();
	_particles->render();
	graphics::selectViewport(0);
	sprites->begin();
	_hud->render();
	sprites->end();
	/*
	ds::renderer::selectViewport(_viewport_id);
	drawBorder();
	ds::renderer::selectViewport(_basic_viewport);
	//if (_showSettings) {
		//_context->settings->showCompleteDialog(&_dialog_pos);
	//}
	if (_showDebug) {
		_context->debugPanel.showDialog(&_dialog_pos);
	}
	_clock->render();
	_points->render();
	ds::renderer::selectViewport(_viewport_id);
	_context->particles->render();
	//_balls->render();
	if (_grabbing) {
		v2 bp = _world->getPosition(_bomb_id);
		v2 diff = _context->world_pos - bp;
		v2 n = normalize(diff);
		float angle = ds::vector::calculateRotation(n);
		angle += PI;
		ds::vector::addRadial(bp, 50.0f, angle);
		// FIXME: pulsate color
		sprites->draw(bp, math::buildTexture(40, 400, 40, 40),angle);
	}
	_context->world->render();
	_bombs->render();
	
	if (!_dying) {
		//sprites->draw(_context->world_pos, math::buildTexture(40, 0, 40, 42), _context->playerAngle);
		//sprites->draw(_context->world_pos, math::buildTexture(440, 0, 152, 152));
	}


	ds::renderer::selectViewport(0);
	sprites->draw(_cursor_pos, math::buildTexture(40, 160, 20, 20));
	
	*/
}

// ---------------------------------------
// move towards player if in range
// ---------------------------------------
void MainGameState::moveStars(const v2& target, float dt) {
	ID ids[64];
	int num = _world->find_by_type(OT_STAR, ids, 64);
	for (int i = 0; i < num; ++i) {
		const v2 p = _world->getPosition(ids[i]).xy();
		v2 diff = target - p;
		if (sqr_length(diff) <  _context->settings->starMagnetRadius * _context->settings->starMagnetRadius) {
			v2 n = normalize(diff);
			n *= _context->settings->starSeekVelocity;
			v2 np = p + n * dt;
			_world->setPosition(ids[i], np);
		}
	}
}

// ---------------------------------------
// create star
// ---------------------------------------
void MainGameState::createStar(const v2& pos) {
	ID id = _world->create(pos, math::buildTexture(0, 40, 24, 24), OT_STAR, 0.0f, v2(1, 1), ds::Color(255, 180, 0, 255));
	_world->scaleByPath(id, &_context->settings->starScalePath, _context->settings->starFlashTTL);
	_world->attachCollider(id, ds::PST_CIRCLE, v2(24.0f, 24.0f));
	_world->removeAfter(id, _context->settings->starTTL);
}

// ---------------------------------------
// add new star
// ---------------------------------------
void MainGameState::addStar(const v2& pos, int count) {
	ID ids[64];
	int num = _world->find_by_type(OT_STAR, ids, 64);
	if (num + count < 128) {
		if (count == 1) {
			createStar(pos);
		}
		else {
			// spread out with radius = 20
			float step = TWO_PI / static_cast<float>(count);
			for (int i = 0; i < count; ++i) {
				v2 position;
				position.x = pos.x + 20.0f * cos(step * static_cast<float>(i));
				position.y = pos.y + 20.0f * sin(step * static_cast<float>(i));
				createStar(position);
			}
		}
	}
}

// -------------------------------------------------------
// on char
// -------------------------------------------------------
int MainGameState::onChar(int ascii) {	
	if (ascii == 'e') {
		return 1;
	}
	if (ascii == 'x') {
		//_balls->killAll();
	}
	//if (ascii == 'd') {
		//_showDebug = !_showDebug;
	//}
	if (ascii == '+') {
		++_level;
		_hud->setNumber(2, _level);
	}
	if (ascii == '-') {
		--_level;
		if (_level < 0) {
			_level = 0;
		}
		_hud->setNumber(2, _level);
	}
	if (ascii == '3') {
		//_followerCubes->create(_player,13);
	}
	if (ascii == '4') {
		addStar(v2(math::random(100,800), math::random(100,600)), 1);
	}
	if (ascii == '5') {
		addStar(v2(math::random(100, 800), math::random(100, 600)), 5);
	}
	if (ascii == '6') {
		for (int i = 0; i < 5; ++i) {
			float x = math::random(100.0f, 900.0f);
			float y = math::random(100.0f, 620.0f);
			_particles->start(9, v2(x,y));
			//_particles->start(6, v2(x, y));
		}
	}
	if (ascii == '8') {
		_levels->start(_level);
	}
	return 0;
}

