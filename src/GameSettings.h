#pragma once
#include <core\data\DynamicSettings.h>

struct GameSettings : public ds::DynamicGameSettings {

	float bombFlashingTTL;
	// star
	float starTTL;
	float starFlashTTL;
	float starMagnetRadius;
	float starSeekVelocity;
	ds::V3Path starScalePath;
	// bomb
	float bombStartTTL;
	int maxBombs;

	GameSettings() : ds::DynamicGameSettings("content\\game_settings.json") {
		add("settings.bomb_flashing_ttl", &bombFlashingTTL, 0.8f);
		add("settings.star_ttl", &starTTL, 3.0f);
		add("settings.star_flash_ttl", &starFlashTTL, 0.6f);
		add("settings.star_magnet_radius", &starMagnetRadius, 250.0f);
		add("settings.star_seek_velocity", &starSeekVelocity, 400.0f);
		add("settings.bomb_start_ttl", &bombStartTTL, 0.5f);
		add("settings.star_scale_path", &starScalePath);
		add("settings.max_bombs", &maxBombs, 3);
	}

};
