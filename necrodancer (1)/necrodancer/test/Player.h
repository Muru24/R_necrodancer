#pragma once
#include "UnitBase.h"

class Map;

class Player : public UnitBase
{
private:
	bool  m_prevKeyState[4];

public:
	Player() {
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = false;
	}
	Player(int hp, int attack, int movespeed, Vector2 pos, ObjectTag tag)
		: UnitBase(hp, attack, movespeed, pos, tag) {
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = false;
	}

	virtual void Move();

	virtual void Attack(UnitBase& Target);

	virtual void TakeDamage(int atk);

	virtual void Die();

	virtual void Update();
};
