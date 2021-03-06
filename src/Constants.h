#pragma once
#include <core\graphics\Color.h>
#include <renderer\render_types.h>

const int MAX_BALLS = 256;

const int HUD_POINTS = 0;
const int HUD_LEVEL  = 1;
const int HUD_TIMER  = 2;

const float PLAYER_RADIUS = 22.0f;
const float BOMB_EXPLOSION_RADIUS = 100.0f;

const int SEEK_BIT        = 1;
const int SEPARATE_BIT    = 2;
const int ALIGN_BIT       = 3;
const int SIMPLE_MOVE_BIT = 4;

const int DODGER_EXPLOSION    = 5;
const int PLAYER_EXPLOSION    = 5;
const int BOMB_EXPLOSION      = 5;
const int BOMB_RING_EXPLOSION = 9;
const int BOMB_DEBRIS         = 10;
const int BALL_EXPLOSION      = 7;
const int BOMB_STARTUP        = 11;
const int BIG_CUBE_EXPLOSION  = 6;
const int HUGE_CUBE_EXPLOSION = 6;
const int ASTEROID_EXPLOSION = 17;
const int ASTEROID_STREAKS   = 16;
const int ASTEROID_RING      = 18;

// objects types
enum ObjectType {
	OT_UNKNOWN,
	OT_STAR,
	OT_PLAYER,
	OT_FOLLOWER,
	OT_WIGGLE_FOLLOWER,
	OT_RING,
	OT_WANDERER,
	OT_SPOTTER,
	OT_BULLET,
	OT_WALL,
	OT_BULLET_TRAIL,
	OT_EOL
};