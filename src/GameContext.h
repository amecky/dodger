#pragma once
#include <dialogs\GUIDialog.h>
#include "Constants.h"
#include "GameSettings.h"
#include <particles\ParticleManager.h>

struct GameContext {

	GameSettings* settings;
	ds::GUIDialog* hudDialog;
	v2 playerPosition;
	float playerAngle;
	ds::ParticleManager* particles;

	int points;
};

