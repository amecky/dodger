#include "FourierPathContainer.h"

bool FourierPathContainer::loadData(const ds::JSONReader& loader) {
	int entries[128];
	int num = loader.get_categories(entries, 128);
	for (int i = 0; i < num; ++i) {
		FourierPath path;
		loader.get_int(entries[i],"num", &path.num);
		loader.get(entries[i], "v1", &path.values[0]);
		loader.get(entries[i], "v2", &path.values[1]);
		loader.get(entries[i], "v3", &path.values[2]);
		loader.get(entries[i], "v4", &path.values[3]);
		loader.get(entries[i], "v5", &path.values[4]);
		loader.get(entries[i], "v6", &path.values[5]);
		loader.get(entries[i], "v7", &path.values[6]);
		loader.get(entries[i], "v8", &path.values[7]);
		loader.get(entries[i], "height", &path.height);
		loader.get(entries[i], "intervall", &path.intervall);
		loader.get(entries[i], "speed", &path.speed);
		path.totalTime = 1280.0f / path.speed;
		path.ttl = path.totalTime / path.intervall;		
		path.min = 10000.0f;
		path.max = -10000.0f;
		for (int j = 0; j < 256; ++j) {
			float t = static_cast<float>(j) / 256.0f * path.intervall * PI;
			float v = 0.0f;
			for (int k = 0; k < path.num; ++k) {
				if (path.values[k] > 0.0f) {
					float f = static_cast<float>(k) * 2.0f + 1.0f;
					v += path.values[k] / PI * sin(f * t);
				}
			}
			if (v > path.max) {
				path.max = v;
			}
			if (v < path.min) {
				path.min = v;
			}
		}
		LOG << "path " << i << " total: " << path.totalTime << " ttl: " << path.ttl << " min: " << path.min << " max: " << path.max;
		_pathList.push_back(path);
	}
	return true;
}

bool FourierPathContainer::reloadData(const ds::JSONReader& loader) {
	_pathList.clear();
	return loadData(loader);
}
