#include "Numbers.h"
#include <core\math\math.h>


const float ND_START[] = {
	408.0f,456.0f, 491.0f, 535.0f, 574.0f, 617.0f, 661.0f, 704.0f, 747.0f, 789.0f
};

const float ND_WIDTH[] = {
	38.0f, 28.0f, 35.0f, 30.0f , 34.0f , 36.0f , 36.0f , 36.0f , 33.0f , 34.0f
};


Numbers::Numbers(const NumberDefinitions& definitions,int length) : _length(length) {
	for (int i = 0; i < 10; ++i) {
		_definitions[i].texture = math::buildTexture(definitions.rects[i]);
		_definitions[i].width = definitions.rects[i].width();
	}
	num = 0;
}


Numbers::~Numbers() {
}

void Numbers::set(const v2& pos, int value, const ds::Color& color) {
	// determine length
	int div = 1;
	if (_length == -1) {
		_length = 0;
		int tmp = value / div;
		while ( tmp >= 1) {
			div *= 10;
			tmp = value / div;
			++_length;
		}
	}
	if (_length >= 10) {
		_length = 10;
	}
	if (_length == 0) {
		_length = 1;
	}
	div = pow(10, (_length - 1));
	//LOG << "value: " << value << " length: " << length << " div: " << div;
	Vector2f p = pos;
	int tmp = value;
	float xp = pos.x;
	for (int i = 0; i < _length; ++i) {
		int r = tmp / div;
		if (r < 0 || r > 9) {
			r = 0;
		}
		m_Sprites[i].texture = _definitions[r].texture;
		p.x = xp + _definitions[r].width * 0.5f;
		m_Sprites[i].position = p;
		m_Sprites[i].color = color;
		// sp.position = (Vector2f(x + dimX * 0.5f, y + dimY * 0.5f));
		//p.x += _definitions[r].width;
		xp += _definitions[r].width;
		tmp = tmp - r * div;
		div /= 10;
	}
	m_Dimension.x = xp;
	m_Dimension.y = _definitions[0].texture.dim.y;
	num = _length;
}

void Numbers::render() {
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	for (int i = 0; i < num; ++i) {
		sprites->draw(m_Sprites[i]);// .position, m_Sprites[i].texture.textureID, m_Sprites[i].texture.uv, m_Sprites[i].texture.dim, 0.0f, 1.0f, 1.0f, m_Sprites[i].color);
	}
}