#pragma once
#include "UnitBase.h"

class Map;

class Slime : public UnitBase
{
private:
	int MoveCount = 0;
	int MaxMoveCount = 0;
public:
	Slime(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag);
	virtual ~Slime() {}

	virtual void Move();
	virtual void Attack(UnitBase& Target);
	virtual void TakeDamage(int atk);
	virtual void Die();
	virtual void Update();
};
