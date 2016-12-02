#include "InfectedGrid.h"
#include <core\math\hex.h>

InfectedGrid::InfectedGrid() {
	_grid.resize(16, 16);
	Layout l(layout_pointy, v2(20,20),v2(640, 360));
	_grid.setLayout(l);
}


InfectedGrid::~InfectedGrid() {
}
