#include "UnitBase.h"

void UnitBase::Attack(UnitBase& Target)
{
	Target.TakeDamage(GetAttack());
}

void UnitBase::TakeDamage(int atk)
{
	status.Hp -= atk;
	if (status.Hp <= 0)
	{
		status.IsAlive = false;
		Die();
	}
}

void UnitBase::Die()
{
}
