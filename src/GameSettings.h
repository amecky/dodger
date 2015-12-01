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

	GameSettings() {
		addFloat("ballGrowTTL", &ballGrowTTL, 1.2f);
		addFloat("gateFlashingTTL", &gateFlashingTTL, 0.8f);
		addFloat("starTTL", &starTTL, 3.0f);
		addFloat("starMagnetRadius", &starMagnetRadius, 250.0f);
		addFloat("starBaseScale", &starBaseScale, 0.95f);
		addFloat("starScaleAmplitude", &starScaleAmplitude, 6.0f);
		addFloat("starScaleFactor", &starScaleFactor, 0.05f);
		addFloat("starSeekVelocity", &starSeekVelocity, 400.0f);
	}

};
