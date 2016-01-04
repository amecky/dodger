#include "Player.h"
#include <sprites\SpriteBatch.h>

Player::Player(GameContext* ctx) : _context(ctx) , _dying(false) {
}


Player::~Player() {
}

// -------------------------------------------------------
// move player
// -------------------------------------------------------
void Player::move(float dt) {
	_cursor_pos = ds::renderer::getMousePosition();
	_context->debugPanel.show("Cursor", _cursor_pos);
	v2 wp;
	float dx = _context->world_pos.x - 1280.0f / 2.0f;
	if (dx < 0.0f) {
		dx = 0.0f;
	}
	if (dx > 320.0f) {
		dx = 320.0f;
	}
	wp.x = _cursor_pos.x + dx;

	float dy = _context->world_pos.y - 720.0f / 2.0f;
	if (dy < 0.0f) {
		dy = 0.0f;
	}
	if (dy > 180.0f) {
		dy = 180.0f;
	}
	wp.y = _cursor_pos.y + dy;
	_context->debugPanel.show("WP", wp);

	ds::math::followRelative(wp, _context->playerPosition, &_context->playerAngle, 5.0f, 1.1f * dt);
	ds::vector::clamp(_context->playerPosition, v2(60, 60), v2(1540, 840));
	_context->world_pos = _context->playerPosition;
	ds::renderer::setViewportPosition(_context->viewport_id, _context->world_pos);
}

void Player::render() {
	ds::renderer::selectViewport(_context->viewport_id);
	if (!_dying) {
		ds::sprites::draw(_context->world_pos, ds::math::buildTexture(40, 0, 40, 42), _context->playerAngle);
		ds::sprites::draw(_context->world_pos, ds::math::buildTexture(440, 0, 152, 152));
	}
	ds::renderer::selectViewport(0);
	ds::sprites::draw(_cursor_pos, ds::math::buildTexture(40, 160, 20, 20));
}

void Player::reset() {
	_context->playerPosition = v2(800, 450);
	_context->playerAngle = 0.0f;
	_cursor_pos = v2(640, 360);
}