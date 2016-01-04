#include "Cubes.h"

bool CubeDefinitions::loadData(JSONReader& reader) {
	std::vector<Category*> all = reader.getCategories();
	for (size_t i = 0; i < all.size(); ++i) {
		Category* c = all[i];
		if (c->getName() == "cube") {
			CubeDefinition def;
			const char* t = c->getProperty("name");
			sprintf_s(def.name, 20, "%s", t);
			def.velocity = c->getFloat("velocity",0.0f);
			def.velocityVariance = c->getFloat("velocity_variance", 0.0f);
			def.numStars = c->getInt("num_stars", 1);
			def.behaviorBits = c->getInt("behavior_bits", 1);
			def.growTTL = c->getFloat("grow_ttl", 0.2f);
			_definitions.push_back(def);
		}
	}
	return true;
}

Cubes::Cubes()
{
}


Cubes::~Cubes()
{
}
