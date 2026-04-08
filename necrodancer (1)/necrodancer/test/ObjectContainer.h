#pragma once
#include <vector>
#include "UnitBase.h"

class ObjectContainer
{
private:
	std::vector<UnitBase*> UnitContainer;
	std::vector<AttackEffect*> m_attackEffects;

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

	void AddAttackEffect(ItemID id, Vector2 pos, float angle);
	void UpdateEffects(float deltaTime);
	const std::vector<AttackEffect*>& GetAttackEffects() { return m_attackEffects; }
	void ClearEffects();
};
