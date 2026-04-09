#pragma once
#include "UnitBase.h"

class Skeleton : public UnitBase
{
private:
	int m_beatCount;

public:
	Skeleton(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag);
	virtual ~Skeleton() {}

	virtual void Move();
	virtual void Die();
	virtual void Update();
};
