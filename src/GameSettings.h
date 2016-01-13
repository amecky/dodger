#pragma once
#include <data\DynamicSettings.h>

struct GameSettings : public ds::DynamicGameSettings {

	float bombFlashingTTL;
	// star
	float starTTL;
	float starMagnetRadius;
	float starSeekVelocity;
	// bomb
	float bombStartTTL;
	int maxBombs;

	GameSettings() {
		addFloat("bomb_flashing_ttl", &bombFlashingTTL, 0.8f);
		addFloat("star_ttl", &starTTL, 3.0f);
		addFloat("star_magnet_radius", &starMagnetRadius, 250.0f);
		addFloat("star_seek_velocity", &starSeekVelocity, 400.0f);
		addFloat("bomb_start_ttl", &bombStartTTL, 0.5f);
		addInt("max_bombs", &maxBombs, 3);
	}

	const char* getFileName() const {
		return "game_settings.json";
	}
};
