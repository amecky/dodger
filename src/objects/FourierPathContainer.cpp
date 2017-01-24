#include "FourierPathContainer.h"
#include <core\math\StraightPath.h>
#include <core\math\CubicBezierPath.h>

bool FourierPathContainer::loadData(const ds::JSONReader& loader) {
	int entries[128];
	int num = loader.get_categories(entries, 128);
	for (int i = 0; i < num; ++i) {
		const char* type = loader.get_string(entries[i], "type");
		ds::Path* path = 0;
		if (strcmp(type, "fourier") == 0) {
			path = new ds::FourierPath;
		}
		else if (strcmp(type, "straight") == 0) {
			path = new ds::StraightPath;
		}
		else if (strcmp(type, "cubic_bezier") == 0) {
			path = new ds::CubicBezierPath;
		}
		if (path != 0) {
			path->loadData(loader, entries[i]);
			float ttl = 0.0f;
			loader.get(entries[i], "ttl", &ttl);
			_ttls.push_back(ttl);
			_pathList.push_back(path);
		}
	}
	return true;
}

bool FourierPathContainer::reloadData(const ds::JSONReader& loader) {
	_pathList.clear();
	return loadData(loader);
}
