#include "Unit.h"

void Unit::Move()
{
	// 하위 클래스에서 각자의 이동 로직을 오버라이딩하여 정의합니다.
}

// 대상에게 피해를 입힙니다.
// - Target: 공격을 받을 대상 유닛
void Unit::Attack(Unit& Target)
{
	Target.TakeDamage(GetAttack());
}

// 피해를 입고 체력이 0 이하가 될 경우 Die() 메서드를 호출합니다.
// - atk: 유닛이 받는 최종 피해량
void Unit::TakeDamage(int atk)
{
	status.Hp -= atk;
	if (status.Hp <= 0)
	{
		status.IsAlive = false;
		Die();
	}
}

void Unit::Die()
{
	// 하위 클래스에서 유닛 파손 또는 소멸 시의 동작을 정의합니다.
}
