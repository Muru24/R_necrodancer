#pragma once
#include "UnitBase.h"

class Shopkeeper : public UnitBase
{
private:
	bool m_isAttack;

public:
	Shopkeeper(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag);
	virtual ~Shopkeeper();

	virtual void Move() override;
	virtual void Attack(UnitBase& Target) override;
	
	bool GetIsAttack() const { return m_isAttack; }
	void SetIsAttack(bool isAttack) { m_isAttack = isAttack; }
};
