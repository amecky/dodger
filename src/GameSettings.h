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

	struct Wanderer {
		float minRotationAngle;
		float maxRotationAngle;
		float minRotationTTL;
		float maxRotationTTL;
		float minScaleTTL;
		float maxScaleTTL;
		float minVelocity;
		float maxVelocity;
		float minMoveTTL;
		float maxMoveTTL;
	} wanderer;

	struct Bullets {
		float velocity;
		float rate;
	} bullets;

	struct Grid {
		float ttl;
		ds::Color flash;
		ds::Color regular;
	} grid;

	GameSettings() : ds::DynamicGameSettings("content\\game_settings.json") {
		add("settings.bomb_flashing_ttl", &bombFlashingTTL, 0.8f);
		add("settings.star_ttl", &starTTL, 3.0f);
		add("settings.star_flash_ttl", &starFlashTTL, 0.6f);
		add("settings.star_magnet_radius", &starMagnetRadius, 250.0f);
		add("settings.star_seek_velocity", &starSeekVelocity, 400.0f);
		add("settings.bomb_start_ttl", &bombStartTTL, 0.5f);
		add("settings.star_scale_path", &starScalePath);
		add("settings.max_bombs", &maxBombs, 3);

		add("wanderer.min_rotation_angle", &wanderer.minRotationAngle, 45.0f);
		add("wanderer.max_rotation_angle", &wanderer.maxRotationAngle, 180.0f);
		add("wanderer.min_rotation_ttl", &wanderer.minRotationTTL, 1.0f);
		add("wanderer.max_rotation_ttl", &wanderer.maxRotationTTL, 2.0f);
		add("wanderer.min_scale_ttl", &wanderer.minScaleTTL, 1.0f);
		add("wanderer.max_scale_ttl", &wanderer.maxScaleTTL, 2.0f);
		add("wanderer.min_velocity", &wanderer.minVelocity, 40.0f);
		add("wanderer.max_velocity", &wanderer.maxVelocity, 200.0f);
		add("wanderer.min_move_ttl", &wanderer.minMoveTTL, 1.0f);
		add("wanderer.max_move_ttl", &wanderer.maxMoveTTL, 2.0f);

		add("bullets.velocity", &bullets.velocity, 400.0f);
		add("bullets.rate", &bullets.rate, 0.2f);

		add("grid.ttl", &grid.ttl, 0.2f);
		add("grid.regular_color", &grid.regular, ds::Color(32, 32, 32, 255));
		add("grid.flash_color", &grid.flash, ds::Color(32, 32, 32, 255));
	}

};
