#pragma once
#include <core\io\DataFile.h>

struct FourierPath {
	int num;
	float values[8];
	float height;
	float intervall;
	float speed;
	float totalTime;
	float ttl;
	float min;
	float max;
};

typedef ds::Array<FourierPath> FourierPathList;

class FourierPathContainer : public ds::JSONAssetFile {

public:
	FourierPathContainer() : ds::JSONAssetFile("content\\resources\\f_path.json") {}
	~FourierPathContainer() {}
	bool loadData(const ds::JSONReader& loader);
	bool reloadData(const ds::JSONReader& loader);
	const FourierPath& get(int index) const {
		return _pathList[index];
	}
	FourierPath& get(int index) {
		return _pathList[index];
	}
private:
	FourierPathList _pathList;
};

