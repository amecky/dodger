#pragma once
#include <dialogs\GUIDialog.h>
#include "Constants.h"
#include "GameSettings.h"
#include <particles\ParticleManager.h>
#include <core\world\World.h>

struct GameContext {

	GameSettings* settings;
	ds::ParticleManager* particles;	
	int points;
	ds::World* world;
	RID additiveBlendState; //
};

