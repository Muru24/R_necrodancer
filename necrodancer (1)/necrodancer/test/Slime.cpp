#include "Slime.h"
#include "Map.h"
#include "MonsterMovePatten.h"
#include <Windows.h>
#include "MainGame.h"
#include "Define.h"
#include <cmath>
#include <iostream>

Slime::Slime(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag)
	: UnitBase(hp, attack, movedistance, pos, tag), MoveCount(0), MaxMoveCount(0)
{
}

void Slime::Move()
{
	if (GetIsMoving()) return;

	Map* pMap = MainGame::getInstance().GetMap();
	if (!pMap) return;

	int dx = 0, dy = 0;
	if (Slime_Move_Pattens.empty() || Slime_Move_Pattens[0].empty()) return;

	Vector2 dir = Slime_Move_Pattens[0][MoveCount];
	MaxMoveCount = (int)Slime_Move_Pattens[0].size();

	dx = (int)dir.X;
	dy = (int)dir.Y;

	if (TryMove(dx, dy))
	{
		MoveCount = (MoveCount + 1) % MaxMoveCount;
	}
}

void Slime::Attack(UnitBase& Target)
{
	if (GetIsAlive())
		UnitBase::Attack(Target);
}

void Slime::TakeDamage(int atk)
{
	UnitBase::TakeDamage(atk);
}

void Slime::Die()
{
}

void Slime::Update()
{
	UnitBase::Update();
}
