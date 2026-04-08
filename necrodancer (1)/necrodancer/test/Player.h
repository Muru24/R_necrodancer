#pragma once
#include "UnitBase.h"

#include <vector>
#include <utility>

class Map;
class ItemBase;

class Player : public UnitBase
{
private:
	int m_Money;
	int m_Bombs;
	bool  m_prevKeyState[4];
	int   m_visionRadius;
	bool  m_bActedThisBeat = false;
	std::vector<std::pair<ItemSlot, ItemBase*>> m_equips;


public:
	Player() : m_Money(0), m_Bombs(1), m_visionRadius(3), m_bActedThisBeat(false) {
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = false;
	}
	Player(int hp, int attack, int movespeed, Vector2 pos, ObjectTag tag)
		: UnitBase(hp, attack, movespeed, pos, tag), m_Money(0), m_Bombs(1), m_visionRadius(3), m_bActedThisBeat(false) {
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = false;
	}

	virtual void Move();

	virtual void Attack(UnitBase& Target);

	virtual void TakeDamage(float atk) override;

	virtual void Die();

	int GetVisionRadius() const { return m_visionRadius; }
	void SetVisionRadius(int radius) { m_visionRadius = radius; }

	void Equip(ItemBase* pItem);
	void DestroyItem(ItemBase* pItem);
	ItemBase* GetEquippedItem(ItemSlot slot) const;
	virtual float GetProtection() const override;

	float GetDigLevel() const;

	virtual void Update();

	void SetMoney(int money) { m_Money += money; }
	int GetMoney()const { return m_Money; }

	int GetBombs() const { return m_Bombs; }
	void AddBombs(int amount) { m_Bombs += amount; }
};
