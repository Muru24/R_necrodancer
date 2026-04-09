#pragma once
#include "Struct.h"
#include <vector>

class UtilPathFinder
{
public:

	static Vector2 FindNextStepAStar(Vector2 startGrid, Vector2 targetGrid);
};
