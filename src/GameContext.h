#pragma once
#include <dialogs\GUIDialog.h>
#include "Constants.h"
#include "GameSettings.h"

struct GameContext {

	GameSettings* settings;
	ds::GUIDialog* hudDialog;
	v2 playerPosition;
};

