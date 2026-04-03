#pragma once
#include "UnitBase.h"

class Map;

class Sliem : public UnitBase
{
private:
	int MoveCount = 0;
	int MaxMoveCount;
public:
	Sliem(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag);
	virtual ~Sliem() {}

	virtual void Move();
	virtual void Attack(UnitBase& Target);
	virtual void TakeDamage(int atk);
	virtual void Die();
	virtual void Update();
};
