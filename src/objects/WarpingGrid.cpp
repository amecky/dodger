#include "WarpingGrid.h"
#include <resources\ResourceContainer.h>

const float TIME_STEPSIZE2 = 0.95f * 0.95f;
const v3 DOT_POINTS[] = { v3(-2, 2, 0), v3(2, 2, 0), v3(2, -2, 0), v3(-2, -2, 0) };

WarpingGrid::WarpingGrid(GameSettings* settings) : _settings(settings) {
	_dotTex = math::buildTexture(40, 120, 4, 4);
	_lineTex = math::buildTexture(42, 122, 10, 2);
	_squareBuffer = ds::res::find("Squares", ds::ResourceType::SQUAREBUFFER);
	_useSquares = false;
}

WarpingGrid::~WarpingGrid() {
}

// -------------------------------------------------------
// create grid
// -------------------------------------------------------
void WarpingGrid::createGrid() {
	for (int y = 0; y < GRID_DIM_Y; ++y) {
		for (int x = 0; x < GRID_DIM_X; ++x) {
			GridPoint& gp = _grid[x][y];
			gp.acceleration = v2(0.0f);
			gp.damping = 0.98f;
			gp.invMass = 1.0f;
			gp.movable = true;
			if (x == 0 || y == 0 || x == (GRID_DIM_X - 1) || y == (GRID_DIM_Y - 1)) {
				gp.invMass = 0.0f;
				gp.movable = false;
			}
			gp.velocity = v2(0.0f);
			gp.pos = v2(2.0f + x * GRID_SIZE, 50.0f + y * GRID_SIZE);
			gp.old_pos = gp.pos;
			gp.color = _settings->grid.regular;
			gp.fading = false;
			gp.timer = 0.0f;
		}
	}

	for (int y = 0; y < GRID_DIM_Y; ++y) {
		for (int x = 0; x < GRID_DIM_X; ++x) {
			/*
			if (x == 0 || y == 0 || x == (GRID_DIM_X - 1) || y == (GRID_DIM_Y - 1)) { // anchor the border of the grid 
				addSpring(p2i(x,y),p2i(x,y), 0.1f, 0.1f);
			}
			else if (x % 3 == 0 && y % 3 == 0)                                  // loosely anchor 1/9th of the point masses 
				addSpring(p2i(x,y),p2i(x,y),0.002f, 0.02f);
			*/
			const float stiffness = 0.28f;
			const float damping = 0.06f;
			if (x > 0)
				addSpring(p2i(x-1,y), p2i(x,y), stiffness, damping);
			if (y > 0)
				addSpring(p2i(x, y - 1), p2i(x, y), stiffness, damping);
		}
	}

}

// -------------------------------------------------------
// add spring
// -------------------------------------------------------
void WarpingGrid::addSpring(p2i start, p2i end, float stiffness, float damping) {
	Spring spring;
	spring.start = start;
	spring.end = end;
	spring.stiffness = stiffness;
	spring.damping = damping;
	const GridPoint& sp = _grid[start.x][start.y];
	const GridPoint& ep = _grid[end.x][end.y];
	spring.targetLength = distance(sp.pos, ep.pos);// *0.95f;
	_springs.push_back(spring);
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
void WarpingGrid::tick(float dt) {	
	ZoneTracker z("WarpingGrid::tick");
	for (int y = 0; y < GRID_DIM_Y; ++y) {
		for (int x = 0; x < GRID_DIM_X; ++x) {
			GridPoint& gp = _grid[x][y];
			if (gp.movable) {
				v2 temp = gp.pos;
				gp.pos = gp.pos + (gp.pos - gp.old_pos) * gp.damping + gp.acceleration * TIME_STEPSIZE2;
				gp.old_pos = temp;
				gp.acceleration = v2(0.0f);
			}
			if (gp.fading) {
				gp.timer += dt;
				gp.color = ds::color::lerp(_settings->grid.flash, _settings->grid.regular, gp.timer/_settings->grid.ttl);
				if (gp.timer >= _settings->grid.ttl) {
					gp.fading = false;
					gp.color = _settings->grid.regular;
				}
			}
		}
	}

	for (uint32_t i = 0; i < _springs.size(); ++i) {
		Spring& spring = _springs[i];
		GridPoint& sp = _grid[spring.start.x][spring.start.y];
		GridPoint& ep = _grid[spring.end.x][spring.end.y];
		v2 delta = ep.pos - sp.pos; // vector from p1 to p2
		float current_distance = length(delta); // current distance between p1 and p2
		/*
		v2 correctionVector = delta * (1 - spring.targetLength / current_distance); // The offset vector that could moves p1 into a distance of rest_distance to p2
		v2 correctionVectorHalf = correctionVector * 0.5f; // Lets make it half that length, so that we can move BOTH p1 and p2.
		if (sp.movable) {
			sp.pos += correctionVectorHalf; // correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the constraint.
		}
		if (ep.movable) {
			ep.pos -= correctionVectorHalf;
		}
		*/
		v2 x = sp.pos - ep.pos;
		float l = length(x);
		if (l > spring.targetLength) {
			x = (x / l) * (l - spring.targetLength);
			v2 dv = ep.velocity - sp.velocity;
			v2 force = spring.stiffness * x - dv * spring.damping;
			sp.acceleration -= force;
			ep.acceleration += force;
		}
	}
}

// -------------------------------------------------------
// apply force
// -------------------------------------------------------
void WarpingGrid::applyForce(const v2& pos, float force, float radius) {
	float sr = radius * radius;
	for (int y = 0; y < GRID_DIM_Y; ++y) {
		for (int x = 0; x < GRID_DIM_X; ++x) {
			GridPoint& gp = _grid[x][y];
			if (gp.movable) {
				v2 d = gp.pos - pos;
				if (sqr_length(d) < sr) {
					applyForce(p2i(x, y), d * force);
				}
			}
		}
	}
}

// -------------------------------------------------------
// apply force
// -------------------------------------------------------
void WarpingGrid::applyForce(const v2& pos, float force, float innerRadius, float outerRadius) {
	float isr = innerRadius * innerRadius;
	float osr = outerRadius * outerRadius;
	for (int y = 0; y < GRID_DIM_Y; ++y) {
		for (int x = 0; x < GRID_DIM_X; ++x) {
			GridPoint& gp = _grid[x][y];
			if (gp.movable) {
				v2 d = gp.pos - pos;
				if (sqr_length(d) > isr && sqr_length(d) < osr) {
					applyForce(p2i(x, y), d * force);
					gp.color = ds::Color(128, 0, 0, 255);
					gp.fading = true;
					gp.timer = 0.2f;
				}
			}
		}
	}
}

// -------------------------------------------------------
// applay force
// -------------------------------------------------------
void WarpingGrid::applyForce(p2i p, const v2& f) {
	GridPoint& gp = _grid[p.x][p.y];
	gp.acceleration += f * gp.invMass;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void WarpingGrid::render() {
	ZoneTracker z("WarpingGrid::render");
	if (_useSquares){
		ds::SquareBuffer* squares = ds::res::getSquareBuffer(_squareBuffer);
		squares->begin();
		v3 p[4];

		for (int y = 0; y < GRID_DIM_Y; ++y) {
			for (int x = 0; x < GRID_DIM_X; ++x) {
				const GridPoint& gp = _grid[x][y];
				for (int i = 0; i < 4; ++i) {
					p[i] = v3(gp.pos) + DOT_POINTS[i];
				}
				squares->draw(p, _dotTex, gp.color);
				if (x > 0) {
					squares->drawLine(_grid[x - 1][y].pos, gp.pos, v3(0.0f,-1.0f,0.0f), _lineTex, gp.color);
				}
				if (y > 0) {
					squares->drawLine(_grid[x][y - 1].pos, gp.pos, v3(1.0f,0.0f,0.0f), _lineTex, gp.color);
				}
			}
		}

		squares->end();
	}
	else {
		ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
		for (int y = 0; y < GRID_DIM_Y; ++y) {
			for (int x = 0; x < GRID_DIM_X; ++x) {
				const GridPoint& gp = _grid[x][y];
				sprites->draw(gp.pos, _dotTex, 0.0f, v2(1, 1), gp.color);
				if (x > 0) {
					sprites->drawLine(_grid[x - 1][y].pos, gp.pos, _lineTex, gp.color);
				}
				if (y > 0) {
					sprites->drawLine(_grid[x][y - 1].pos, gp.pos, _lineTex, gp.color);
				}
			}
		}
	}
}