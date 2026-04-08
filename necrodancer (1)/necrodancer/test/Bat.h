#pragma once
#include "UnitBase.h"

class Bat : public UnitBase
{
private:
	int m_beatCount;

public:
	Bat(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag);
	virtual ~Bat() {}

	virtual void Move();
	virtual void Update();
};
