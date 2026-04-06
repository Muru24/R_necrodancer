#pragma once
#include "UnitBase.h"

#include <vector>
#include <utility>

class Map;
class ItemBase;

class Player : public UnitBase
{
private:
	bool  m_prevKeyState[4];
	int   m_visionRadius;
	bool  m_bActedThisBeat = false;
	std::vector<std::pair<ItemSlot, ItemBase*>> m_equips;


public:
	Player() : m_visionRadius(3), m_bActedThisBeat(false) {
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = false;
	}
	Player(int hp, int attack, int movespeed, Vector2 pos, ObjectTag tag)
		: UnitBase(hp, attack, movespeed, pos, tag), m_visionRadius(3), m_bActedThisBeat(false) {
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = false;
	}

	virtual void Move();

	virtual void Attack(UnitBase& Target);

	virtual void TakeDamage(int atk);

	virtual void Die();

	int GetVisionRadius() const { return m_visionRadius; }
	void SetVisionRadius(int radius) { m_visionRadius = radius; }

	void Equip(ItemBase* pItem);
	ItemBase* GetEquippedItem(ItemSlot slot) const;

	int GetDigLevel() const;

	virtual void Update();
};
