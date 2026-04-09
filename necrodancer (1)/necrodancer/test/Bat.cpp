#include "Bat.h"
#include "MainGame.h"
#include <cstdlib>

Bat::Bat(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag)
	: UnitBase(hp, attack, movedistance, pos, tag, MONSTER_BAT), m_beatCount(0)
{
}

void Bat::Move()
{
	if (GetIsMoving()) return;


	m_beatCount++;
	if (m_beatCount < 2) return;
	m_beatCount = 0;


	int dx = (rand() % 3) - 1; // -1, 0, 1
	int dy = (rand() % 3) - 1; // -1, 0, 1

	if (dx == 0 && dy == 0) return;

	TryMove(dx, dy);
}

void Bat::Update()
{
	UnitBase::Update();
}
