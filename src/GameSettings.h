#pragma once
#include <data\DynamicSettings.h>

struct GameSettings : public ds::DynamicGameSettings {

	float ballGrowTTL;
	float gateFlashingTTL;
	// star
	float starTTL;
	float starMagnetRadius;
	float starBaseScale;
	float starScaleAmplitude;
	float starScaleFactor;
	float starSeekVelocity;
	// bomb
	float bombFlashAmplitude;

	float spawnDelay;

	GameSettings() {
		addFloat("ballGrowTTL", &ballGrowTTL, 1.2f);
		addFloat("gateFlashingTTL", &gateFlashingTTL, 1.5f);
		addFloat("starTTL", &starTTL, 3.0f);
		addFloat("starMagnetRadius", &starMagnetRadius, 250.0f);
		addFloat("starBaseScale", &starBaseScale, 0.9f);
		addFloat("starScaleAmplitude", &starScaleAmplitude, 12.0f);
		addFloat("starScaleFactor", &starScaleFactor, 0.1f);
		addFloat("starSeekVelocity", &starSeekVelocity, 400.0f);
		addFloat("spawnDelay", &spawnDelay, 0.5f);
		addFloat("bombFlashAmplitude", &bombFlashAmplitude, 6.0f);
	}

};
