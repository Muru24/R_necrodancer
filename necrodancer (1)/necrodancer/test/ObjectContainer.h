#pragma once
#include <vector>
#include "UnitBase.h"
#include "Struct.h"

class ObjectContainer
{
private:
	std::vector<UnitBase*> UnitContainer;
	std::vector<AttackEffect*> m_attackEffects;
	std::vector<struct Bomb*> m_bombs;
	std::vector<struct ExplosionEffect*> m_explosions;
	std::vector<UnitBase*> m_spawnQueue;

	ObjectContainer() {}

public:
	static ObjectContainer& getInstance() {
		static ObjectContainer instance;
		return instance;
	}
	~ObjectContainer() {}

	const std::vector<UnitBase*>& GetUnitContainer() { return UnitContainer; }

	void PushUnit(UnitBase* unit);
	void PopUnit(int pos);
	void ProcessSpawnQueue();
	void Clear();
	UnitBase* FindUnitAt(int gridX, int gridY);

	void AddAttackEffect(ItemID id, Vector2 pos, float angle);
	void UpdateEffects(float deltaTime);
	const std::vector<AttackEffect*>& GetAttackEffects() { return m_attackEffects; }
	void ClearEffects();

	// Bombs
	void AddBomb(Vector2 pos);
	void UpdateBombs();
	void Explode(struct Bomb* pBomb);
	const std::vector<struct Bomb*>& GetBombs() { return m_bombs; }

	// Explosions
	void AddExplosionEffect(struct ExplosionEffect* pExp);
	void UpdateExplosions(float deltaTime);
	const std::vector<struct ExplosionEffect*>& GetExplosions() { return m_explosions; }
};
