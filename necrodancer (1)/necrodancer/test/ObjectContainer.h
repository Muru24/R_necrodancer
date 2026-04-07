#pragma once
#include <vector>
#include "UnitBase.h"

class ObjectContainer
{
private:
	std::vector<UnitBase*> UnitContainer;

public:
	static ObjectContainer& getInstance() {
		static ObjectContainer instance;
		return instance;
	}
	~ObjectContainer() {}
	const std::vector<UnitBase*>& GetUnitContainer() { return UnitContainer; }

	void PushUnit(UnitBase* unit);
	void PopUnit(int pos);
	void Clear();
	UnitBase* FindUnitAt(int gridX, int gridY);
};
