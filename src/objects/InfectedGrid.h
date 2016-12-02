#pragma once
#include <core\math\HexGrid.h>

class InfectedGrid {

public:
	InfectedGrid();
	~InfectedGrid();
private:
	ds::HexGrid<int> _grid;
};

