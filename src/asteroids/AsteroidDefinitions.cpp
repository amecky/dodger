#include "AsteroidDefinitions.h"



AsteroidDefinitions::AsteroidDefinitions() : ds::JSONAssetFile("content\\resources\\asteroids.json") {
}


AsteroidDefinitions::~AsteroidDefinitions() {
}

bool AsteroidDefinitions::loadData(const ds::JSONReader& loader) {
	int categories[16];
	int num = loader.get_categories(categories, 16);
	for (int i = 0; i < num; ++i) {
		AsteroidInfo info;
		loader.get_int(categories[i], "energy", &info.energy);
		loader.get(categories[i], "min_velocity", &info.minVelocity);
		loader.get(categories[i], "max_velocity", &info.maxVelocity);
		loader.get(categories[i], "radius", &info.radius);
		loader.get_int(categories[i], "particlesID", &info.particlesID);
		const char* objectName = loader.get_string(categories[i], "object_name");
		info.objectHash = StaticHash(objectName);
		_infos.push_back(info);
	}
	return true;
}

bool AsteroidDefinitions::reloadData(const ds::JSONReader& loader) {
	_infos.clear();
	return loadData(loader);
}
