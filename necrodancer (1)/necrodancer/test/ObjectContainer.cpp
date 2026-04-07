#include "ObjectContainer.h"
#include <iostream>
#include <cmath>

void ObjectContainer::PushUnit(UnitBase* unit)
{
	UnitContainer.push_back(unit);
}

void ObjectContainer::PopUnit(int pos)
{
	UnitContainer.erase(UnitContainer.begin()+pos);
}

void ObjectContainer::Clear()
{
	for (auto* unit : UnitContainer)
	{
		if (unit) delete unit;
	}
	UnitContainer.clear();
}

UnitBase* ObjectContainer::FindUnitAt(int gridX, int gridY)
{
	int gridSize = FRAME_SIZE * DRAW_SCALE;
	for (auto* unit : UnitContainer)
	{
		if (!unit || !unit->GetIsAlive()) continue;

		Vector2 logicalPos = unit->GetLogicalPos();
		int uGridX = (int)floor((logicalPos.X + gridSize * 0.5f) / gridSize);
		int uGridY = (int)floor((logicalPos.Y + gridSize * 0.5f) / gridSize);

		if (uGridX == gridX && uGridY == gridY)
		{
			return unit;
		}
	}
	return nullptr;
}
