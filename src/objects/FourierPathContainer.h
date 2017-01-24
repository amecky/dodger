#pragma once
#include <core\io\DataFile.h>
#include <core\math\FourierPath.h>

typedef ds::Array<ds::Path*> PathList;

class FourierPathContainer : public ds::JSONAssetFile {

public:
	FourierPathContainer() : ds::JSONAssetFile("content\\resources\\f_path.json") {}
	~FourierPathContainer() {}
	bool loadData(const ds::JSONReader& loader);
	bool reloadData(const ds::JSONReader& loader);
	const ds::Path* get(int index) const {
		return _pathList[index];
	}
	ds::Path* get(int index) {
		return _pathList[index];
	}
	int num() const {
		return _pathList.size();
	}
	float getTTL(int index) const {
		return _ttls[index];
	}
private:
	PathList _pathList;
	ds::Array<float> _ttls;
};

