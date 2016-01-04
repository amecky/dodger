#pragma once
#include <compiler\DataFile.h>

struct CubeDefinition {

	char name[20];
	float velocity;
	float velocityVariance;
	int numStars;
	int behaviorBits;
	float growTTL;

};

class CubeDefinitions : public ds::DataFile {

public:
	CubeDefinitions() {}
	~CubeDefinitions() {}
	bool saveData(JSONWriter& writer) {
		return true;
	}
	bool loadData(JSONReader& reader);
	const char* getFileName() const {
		return "resources\\cube_definitions.json";
	}
	size_t size() const {
		return _definitions.size();
	}
	const CubeDefinition& get(size_t index) const {
		return _definitions[index];
	}
private:
	std::vector<CubeDefinition> _definitions;
};



class Cubes
{
public:
	Cubes();
	~Cubes();
};

