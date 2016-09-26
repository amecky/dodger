#pragma once
#include <renderer\render_types.h>
#include <renderer\sprites.h>

struct NumberDefinitions {

	ds::Rect rects[10];

	void define(int index, const ds::Rect& r) {
		rects[index] = r;
	}
};

class Numbers {

struct NumberDef {
	float start;
	float width;
	ds::Texture texture;
};

public:
	Numbers(const NumberDefinitions& definitions,int length = -1);
	~Numbers();
	void set(const v2& pos,int value,const ds::Color& color = ds::Color(255,255,255,255));
	void render();
	const v2& getDimension() const {
		return m_Dimension;
	}
private:
	NumberDef _definitions[10];
	ds::Sprite m_Sprites[10];
	ds::Color _color;
	int num;
	int _length;
	Vector2f m_Dimension;
};

