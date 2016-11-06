#pragma once
#include "..\GameContext.h"

const int GRID_DIM_X = 32;
const int GRID_DIM_Y = 17;
const float GRID_SIZE = 40.0f;

struct GridPoint {
	v2 pos;
	v2 old_pos;
	v2 velocity;
	float invMass;
	float damping;
	v2 acceleration;
	bool movable;
};

struct Spring {
	p2i start;
	p2i end;
	float targetLength;
	float stiffness;
	float damping;

};

class WarpingGrid {

public:
	WarpingGrid();
	~WarpingGrid();
	void createGrid();
	void tick(float dt);
	void render();
	void applyForce(p2i p,const v2& f);
	void applyForce(const v2& pos, float force, float radius);
	void applyForce(const v2& pos, float force, float innerRadius, float outerRadius);
private:
	void addSpring(p2i start, p2i end, float stiffness, float damping);
	GridPoint _grid[GRID_DIM_X][GRID_DIM_Y];
	ds::Array<Spring> _springs;
};

