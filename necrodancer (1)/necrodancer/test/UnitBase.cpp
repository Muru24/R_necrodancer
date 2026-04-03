#include "UnitBase.h"
#include "MainGame.h"
#include "Define.h"
#include "Map.h"
#include "ObjectContainer.h"
#include <cmath>
#include <iostream>

void UnitBase::Update()
{
	float deltaTime = MainGame::getInstance().GetDeltaTime();

	if (GetIsMoving()) {
		int speed = MOVE_SPEED;
		if (m_isBumping) speed *= 2;

		float progress = GetMoveProgress() + speed * deltaTime;
		SetMoveProgress(progress);

		if (progress >= 1.0f)
		{
			SetMoveProgress(1.0f);
			if (m_isBumping) {
				SetPos(m_vStartPos);
			} else {
				SetPos(m_vTargetPos);
			}
			SetIsMoving(false);
			m_isBumping = false;
			SetMoveProgress(0.0f);
		}
		else
		{
			Vector2 pos;
			if (m_isBumping) {
				float bumpFactor = sinf(progress * 3.141592f) * 0.3f;
				pos.X = m_vStartPos.X + (m_vTargetPos.X - m_vStartPos.X) * bumpFactor;
				pos.Y = m_vStartPos.Y + (m_vTargetPos.Y - m_vStartPos.Y) * bumpFactor;
			} else {
				pos.X = m_vStartPos.X + (m_vTargetPos.X - m_vStartPos.X) * progress;
				pos.Y = m_vStartPos.Y + (m_vTargetPos.Y - m_vStartPos.Y) * progress;
			}
			SetPos(pos);
		}
	}

	if (GetTickCount() - GetLastAnimTime() >= ANIM_SPEED) {
		SetCurrentFrame((GetCurrentFrame() + 1) % 4);
		SetLastAnimTime(GetTickCount());
	}
}



bool UnitBase::ColliderObject(int gridX, int gridY, int nextX, int nextY)
{
	UnitBase* target = ObjectContainer::getInstance().FindUnitAt(gridX, gridY);
	if (target && target != this) {
		ObjectTag myTag = GetTag();
		ObjectTag targetTag = target->GetTag();

		if (myTag == PLAYER) {
			if (targetTag == ENEMY) {
				Attack(*target);
				m_vStartPos = GetPos();
				m_vTargetPos.X = (float)nextX;
				m_vTargetPos.Y = (float)nextY;
				SetIsMoving(true);
				m_isBumping = true;
				return true;
			}
		}
		else if (myTag == ENEMY) {
			if (targetTag == PLAYER) {
				Attack(*target);
				m_vStartPos = GetPos();
				m_vTargetPos.X = (float)nextX;
				m_vTargetPos.Y = (float)nextY;
				SetIsMoving(true);
				m_isBumping = true;
				return true;
			}
			if (targetTag == ENEMY) {
				m_vStartPos = GetPos();
				m_vTargetPos.X = (float)nextX;
				m_vTargetPos.Y = (float)nextY;
				SetIsMoving(true);
				m_isBumping = true;
				return true;
			}
		}
	}
	return false;
}

void UnitBase::TryMove(int dx, int dy)
{
	if (dx == 0 && dy == 0) return;

	Map* pMap = MainGame::getInstance().GetMap();
	if (!pMap) return;

	int gridSize = FRAME_SIZE * DRAW_SCALE;
	Vector2 logicalPos = GetLogicalPos();
	int nextX = (int)logicalPos.X + dx * gridSize;
	int nextY = (int)logicalPos.Y + dy * gridSize;

	int gridX = nextX / gridSize;
	int gridY = nextY / gridSize;

	TileType t = pMap->GetTile(gridX, gridY).type;
	TileType tTop = pMap->GetTile(gridX, gridY + 1).type;
	bool isWall = (t == TILE_WALL_DEFULT || t == TILE_WALL_HARD || t == TILE_WALL_BADROCK || t == TILE_WALL_SHOP);
	bool isWallTop = (tTop == TILE_WALL_DEFULT || tTop == TILE_WALL_HARD || tTop == TILE_WALL_BADROCK || tTop == TILE_WALL_SHOP);

	if (isWall || isWallTop) {
		m_vStartPos = GetPos();
		m_vTargetPos.X = (float)nextX;
		m_vTargetPos.Y = (float)nextY;
		SetIsMoving(true);
		m_isBumping = true;
		return;
	}

	if (ColliderObject(gridX, gridY,nextX,nextY)) {
		return;
	}

	m_vStartPos = GetPos();
	m_vTargetPos.X = (float)nextX;
	m_vTargetPos.Y = (float)nextY;
	SetIsMoving(true);
	m_isBumping = false;
}

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
