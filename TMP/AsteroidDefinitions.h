#pragma once
#include <core\io\DataFile.h>

struct AsteroidInfo {

	int energy;
	float minVelocity;
	float maxVelocity;
	float rotationVelocity;
	float radius;
	int particles;
	StaticHash objectHash;

	//AsteroidInfo(StaticHash h, int e, float minVel, float maxVel, float r, int p)
	//	: objectHash(h), energy(e), minVelocity(minVel), maxVelocity(maxVel), radius(r), particlesID(p) {}
};


class AsteroidDefinitions : public ds::JSONAssetFile {

public:
	AsteroidDefinitions();
	~AsteroidDefinitions();
	bool loadData(const ds::JSONReader& loader);
	bool reloadData(const ds::JSONReader& loader);
	const AsteroidInfo& getDefinition(int index) const {
		return _infos[index];
	}
private:
	ds::Array<AsteroidInfo> _infos;
};

