#pragma once
#include <data\DynamicSettings.h>

struct GamePlaySettings : public ds::DynamicGameSettings {

	int maxConcurrentBalls;
	int spawnBalls;
	float bigBallEmittTime;
	float hugeBallEmittTime;
	float ballEmittTime;

	GamePlaySettings() {
		addInt("max concurrent balls",&maxConcurrentBalls,100);
		addInt("spawn balls",&spawnBalls, 10);
		addFloat("Big ball emitt time",&bigBallEmittTime, 2.0f);
		addFloat("Huge ball emitt time",&hugeBallEmittTime, 4.0f);
		addFloat("Ball emitt time",&ballEmittTime, 1.5f);
	}

	const char* getName() const {
		return "gameplay_settings";
	}
};

struct GameSettings : public ds::DynamicGameSettings {

	float ballGrowTTL;
	float bombFlashingTTL;
	// star
	float starTTL;
	float starMagnetRadius;
	float starSeekVelocity;
	// bomb
	float bombStartTTL;

	float spawnDelay;

	GameSettings() {
		addFloat("ballGrowTTL", &ballGrowTTL, 1.2f);
		addFloat("Bomb flashing TTL", &bombFlashingTTL, 0.8f);
		addFloat("starTTL", &starTTL, 3.0f);
		addFloat("starMagnetRadius", &starMagnetRadius, 250.0f);
		addFloat("starSeekVelocity", &starSeekVelocity, 400.0f);
		addFloat("spawnDelay", &spawnDelay, 0.5f);
		addFloat("bombStartTTL", &bombStartTTL, 0.5f);
	}

	const char* getName() const {
		return "game_settings";
	}
};
