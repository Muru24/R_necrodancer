#include "Sliem.h"
#include "Map.h"
#include "MonsterMovePatten.h"
#include <Windows.h>
#include "MainGame.h"
#include "Define.h"
#include <cmath>
#include <iostream>

Sliem::Sliem(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag)
	: UnitBase(hp, attack, movedistance, pos, tag)
{
}

void Sliem::Move()
{
	if (GetIsMoving()) return;

	Map* pMap = MainGame::getInstance().GetMap();
	if (!pMap) return;

	int dx = 0, dy = 0;
	int gridSize = FRAME_SIZE * DRAW_SCALE;

	Vector2 dir = Slime_Move_Pattens[0][MoveCount];
	MaxMoveCount = Slime_Move_Pattens[0].size();

	dx = (int)dir.X;
	dy = (int)dir.Y;

	MoveCount++;
	if (MoveCount >= MaxMoveCount) MoveCount = 0;
	if (dx != 0 || dy != 0)
	{
		TryMove(dx, dy);
	}
}

void Sliem::Attack(UnitBase& Target)
{
	if (GetIsAlive())
		UnitBase::Attack(Target);
}

void Sliem::TakeDamage(int atk)
{
	UnitBase::TakeDamage(atk);
}

void Sliem::Die()
{

}

void Sliem::Update()
{
	int oldFrame = GetCurrentFrame();
	UnitBase::Update();

	if (oldFrame != GetCurrentFrame()) {
		Move();
	}
}
