#pragma once
#include "..\GameContext.h"

class AbstractBorder {

	enum LPDirection {
		LP_VERTICAL,
		LP_HORIZONTAL
	};

	struct LinePoint {
		float height;
		float targetHeight;
		float speed;
	};


public:
	AbstractBorder(float target, int num, const ds::Texture texture, const ds::AABBox& box);
	virtual ~AbstractBorder();
	void tick(float dt);
	void render();
	void splash(int index, float speed);
	virtual void buildColumn(int index, v3* positions) = 0;
	bool isInside(const v3& pos) const;
	virtual int getIndex(const v3& pos) const = 0;
protected:
	LinePoint* _points;
	RID _squareBuffer;
	float* lDeltas;
	float* rDeltas;
	int _num;
	float _target;
	ds::Texture _texture;
private:
	ds::AABBox _box;
};

class BottomBorder : public AbstractBorder {

public:
	BottomBorder(float target, int num) : AbstractBorder(target, num, math::buildTexture(0, 302, 36, 40), ds::AABBox(v3(640, 20, 0), v3(1280, 40, 0))) {}
	virtual ~BottomBorder() {}
	void buildColumn(int index, v3* positions) {
		v2 f = v2(20.0f + index * 20.0f, _points[index].height);
		v2 s = v2(20.0f + (index + 1)* 20.0f, _points[index + 1].height);
		positions[0] = v3(f.x, f.y, 0.0f);
		positions[1] = v3(s.x, s.y, 0.0f);
		positions[2] = v3(s.x, 0.0f, 0.0f);
		positions[3] = v3(f.x, 0.0f, 0.0f);
	}
	int getIndex(const v3& pos) const {
		return (pos.x - 20) / 20;
	}
};

class TopBorder : public AbstractBorder {

public:
	TopBorder(float target, int num) : AbstractBorder(target, num, math::buildTexture(0, 342, 36, 40), ds::AABBox(v3(640, 700, 0), v3(1280, 40, 0))) {}
	virtual ~TopBorder() {}
	void buildColumn(int index, v3* positions) {
		v2 f = v2(20.0f + index * 20.0f, _points[index].height);
		v2 s = v2(20.0f + (index + 1)* 20.0f, _points[index + 1].height);
		positions[0] = v3(f.x, 720.0f, 0.0f);
		positions[1] = v3(s.x, 720.0f, 0.0f);
		positions[2] = v3(s.x, 720.0f - s.y, 0.0f);
		positions[3] = v3(f.x, 720.0f - f.y, 0.0f);
	}
	int getIndex(const v3& pos) const {
		return (pos.x - 20) / 20;
	}
};

class LeftBorder : public AbstractBorder {

public:
	LeftBorder(float target, int num) : AbstractBorder(target, num, math::buildTexture(40, 302, 40, 36), ds::AABBox(v3(20, 360, 0), v3(20, 720, 0))) {}
	virtual ~LeftBorder() {}
	void buildColumn(int index, v3* positions) {
		v2 f = v2(_points[index].height,30.0f + index * 20.0f);
		v2 s = v2(_points[index + 1].height, 30.0f + (index + 1)* 20.0f);
		positions[0] = v3(0.0f, s.y, 0.0f);
		positions[1] = v3(s.x, s.y, 0.0f);
		positions[2] = v3(f.x, f.y, 0.0f);
		positions[3] = v3(0.0f, f.y, 0.0f);
	}
	int getIndex(const v3& pos) const {
		return (pos.y - 20) / 20;
	}
};

class RightBorder : public AbstractBorder {

public:
	RightBorder(float target, int num) : AbstractBorder(target, num, math::buildTexture(40, 342, 40, 36), ds::AABBox(v3(1260, 360, 0), v3(20, 720, 0))) {}
	virtual ~RightBorder() {}
	void buildColumn(int index, v3* positions) {
		v2 f = v2(_points[index].height, 30.0f + index * 20.0f);
		v2 s = v2(_points[index + 1].height, 30.0f + (index + 1)* 20.0f);
		positions[0] = v3(1280.0f - s.x, s.y, 0.0f);
		positions[1] = v3(1280.0f, s.y, 0.0f);
		positions[2] = v3(1280.0f, f.y, 0.0f);
		positions[3] = v3(1280.0f - f.x, f.y, 0.0f);
	}
	int getIndex(const v3& pos) const {
		return (pos.y - 20) / 20;
	}
};


class ElasticBorder {

public:
	ElasticBorder();
	~ElasticBorder();
	void tick(float dt);
	void render();
	void splash(const v3& pos, float speed);
private:
	AbstractBorder* _borders[4];
};