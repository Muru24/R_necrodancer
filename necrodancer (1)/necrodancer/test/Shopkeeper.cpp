#include "Shopkeeper.h"

Shopkeeper::Shopkeeper(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag)
	: UnitBase(hp, attack, movedistance, pos, tag), m_isAttack(false)
{
}

Shopkeeper::~Shopkeeper()
{
}

void Shopkeeper::Move()
{
	if (!m_isAttack)
		return;

}

void Shopkeeper::Attack(UnitBase& Target)
{
	if (!m_isAttack)
		return;

}
