#pragma once
#include <dialogs\GUIDialog.h>
#include "Constants.h"
#include "GameSettings.h"
#include <particles\ParticleManager.h>
#include <ui\DebugPanel.h>

struct GameContext {

	GameSettings* settings;
	ds::GUIDialog* hudDialog;
	v2 playerPosition;
	v2 world_pos;
	float playerAngle;
	ds::ParticleManager* particles;
	ds::DebugPanel debugPanel;
	int points;
};

