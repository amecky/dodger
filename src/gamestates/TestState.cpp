#include "TestState.h"
#include <utils\Log.h>
#include <sprites\SpriteBatch.h>
#include <renderer\graphics.h>
#include <base\GameStateMachine.h>
#include "..\objects\Player.h"

TestState::TestState(GameContext* context) : ds::GameState("TestState"), _context(context) {
	_player = new Player(_context);
	ds::renderer::setViewportPosition(_context->viewport_id, v2(960, 540));
	_grid.resize(38, 25);
	_grid.setOrigin(v2(20, 20));
	for (int r = 0; r < _grid.rMax(); r++) {
		int q_offset = r >> 1;
		for (int q = -q_offset; q < _grid.qMax() - q_offset; q++) {
			Hex hex = Hex(q, r);
			GridItem& item = _grid.get(hex);
			item.position = _grid.to_screen(hex);
		}
	}

}


TestState::~TestState() {
	delete _player;
}

// --------------------------------------------
// activate
// --------------------------------------------
void TestState::activate() {
	

}

// --------------------------------------------
// activate
// --------------------------------------------
void TestState::deactivate() {
}

// --------------------------------------------
// update
// --------------------------------------------
int TestState::update(float dt) {
	_player->move(dt);
	return 0;
}

// --------------------------------------------
// click
// --------------------------------------------
int TestState::onGUIButton(ds::DialogID dlgID, int button) {
	return button;
}

// --------------------------------------------
// render
// --------------------------------------------
void TestState::render() {
	ds::renderer::selectViewport(_context->viewport_id);
	for (int i = 0; i < _grid.size(); ++i) {
		const GridItem& item = _grid.get(i);
		ds::sprites::draw(item.position, ds::math::buildTexture(ds::Rect(70, 200, 40, 44)));// , 0.0f, 1.0f, 1.0f, ds::Color(0, 0, 32, 255));
	}
	ds::sprites::draw(v2(60, 60), ds::math::buildTexture(840, 360, 120, 120));
	ds::sprites::draw(v2(1540, 60), ds::math::buildTexture(840, 360, 120, 120));
	ds::sprites::draw(v2(60, 840), ds::math::buildTexture(840, 360, 120, 120));
	ds::sprites::draw(v2(1540, 840), ds::math::buildTexture(840, 360, 120, 120));
	ds::sprites::draw(v2(800, 450), ds::math::buildTexture(840, 360, 120, 120));
	_player->render();
}


