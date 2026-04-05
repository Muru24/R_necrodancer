#pragma once
#include "UnitBase.h"

class Map;

class Player : public UnitBase
{
private:
	bool  m_prevKeyState[4];
	int   m_visionRadius;


public:
	Player() : m_visionRadius(3) {
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = false;
	}
	Player(int hp, int attack, int movespeed, Vector2 pos, ObjectTag tag)
		: UnitBase(hp, attack, movespeed, pos, tag), m_visionRadius(3) {
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = false;
	}

	virtual void Move();

	virtual void Attack(UnitBase& Target);

	virtual void TakeDamage(int atk);

	virtual void Die();

	int GetVisionRadius() const { return m_visionRadius; }
	void SetVisionRadius(int radius) { m_visionRadius = radius; }

	virtual void Update();
};
