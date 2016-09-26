#pragma once
#include <core\data\DynamicSettings.h>

struct GameSettings : public ds::DynamicGameSettings {

	float bombFlashingTTL;
	// star
	float starTTL;
	float starFlashTTL;
	float starMagnetRadius;
	float starSeekVelocity;
	ds::Vector2fPath starScalePath;
	// bomb
	float bombStartTTL;
	int maxBombs;

	GameSettings() : ds::DynamicGameSettings("game_settings.json") {
		add("bomb_flashing_ttl", &bombFlashingTTL, 0.8f);
		add("star_ttl", &starTTL, 3.0f);
		add("star_flash_ttl", &starFlashTTL, 0.6f);
		add("star_magnet_radius", &starMagnetRadius, 250.0f);
		add("star_seek_velocity", &starSeekVelocity, 400.0f);
		add("bomb_start_ttl", &bombStartTTL, 0.5f);
		//addPath("star_scale_path", &starScalePath);
		add("max_bombs", &maxBombs, 3);
	}

};
