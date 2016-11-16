#pragma once
#include "..\GameContext.h"

const int MAX_X_LP = 32;
const int MAX_Y_LP = 17;

class Borders {

	enum LPDirection {
		LP_VERTICAL,
		LP_HORIZONTAL
	};

	struct LinePoint {
		v2 pos;
		float timer;
		float offset;
	};


public:
	Borders();
	~Borders();
	void tick(float dt);
	void render();
private:
	LinePoint _points[MAX_X_LP + MAX_Y_LP];
	RID _squareBuffer;
	int _total;
};

