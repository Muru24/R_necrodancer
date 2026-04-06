#include "UnitBase.h"
#include "MainGame.h"
#include "Define.h"
#include "Map.h"
#include "ObjectContainer.h"
#include "Player.h"
#include <cmath>
#include <iostream>
#include "Item.h"

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

void UnitBase::StartMoving(Vector2 targetPos, bool isBumping)
{
	m_vStartPos = GetPos();
	m_vTargetPos = targetPos;
	SetIsMoving(true);
	m_isBumping = isBumping;
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
				StartMoving({ (float)nextX, (float)nextY }, true);
				return true;
			}
		}
		else if (myTag == ENEMY) {
			if (targetTag == PLAYER) {
				Attack(*target);
				StartMoving({ (float)nextX, (float)nextY }, true);
				return true;
			}
			if (targetTag == ENEMY) {
				StartMoving({ (float)nextX, (float)nextY }, true);
				return true;
			}
		}
	}
	return false;
}

bool UnitBase::TryMove(int dx, int dy)
{
	if (dx == 0 && dy == 0) return true;

	Map* pMap = MainGame::getInstance().GetMap();
	if (!pMap) return false;

	int gridSize = FRAME_SIZE * DRAW_SCALE;
	Vector2 logicalPos = GetLogicalPos();
	int nextX = (int)logicalPos.X + dx * gridSize;
	int nextY = (int)logicalPos.Y + dy * gridSize;

	int gridX = nextX / gridSize;
	int gridY = nextY / gridSize;

	TileType t = pMap->GetTile(gridX, gridY).type;
	TileType tTop = pMap->GetTile(gridX, gridY + 1).type;
	bool isWall = (t == TILE_WALL_DEFAULT || t == TILE_WALL_HARD || t == TILE_WALL_BADROCK || t == TILE_WALL_SHOP);
	bool isWallTop = (tTop == TILE_WALL_DEFAULT || tTop == TILE_WALL_HARD || tTop == TILE_WALL_BADROCK || tTop == TILE_WALL_SHOP);

	if (isWall || isWallTop) {
		if (GetTag() == PLAYER) {
			if (isWallTop)
				pMap->DigTile(gridX, gridY + 1, static_cast<Player*>(this)->GetDigLevel());
			else if (isWall)
				pMap->DigTile(gridX, gridY, static_cast<Player*>(this)->GetDigLevel());
		}
		StartMoving({ (float)nextX, (float)nextY }, true);
		return false;
	}

	if (GetTag() == PLAYER) {
		Player* pPlayer = static_cast<Player*>(this);
		Weapon* pWeapon = static_cast<Weapon*>(pPlayer->GetEquippedItem(SLOT_WEAPON));
		if (pWeapon) {
			const std::vector<Vector2>& rangeList = pWeapon->GetAttackRange();
			bool attacked = false;

			for (const auto& relPos : rangeList) {
				int lx = (int)relPos.X;
				int ly = (int)relPos.Y;

				int targetGridX, targetGridY;
				if (dx != 0) { // 가로 이동
					targetGridX = (int)floor(logicalPos.X / gridSize) + ly * dx;
					targetGridY = (int)floor(logicalPos.Y / gridSize) + lx * dx;
				}
				else { // 세로 이동
					targetGridX = (int)floor(logicalPos.X / gridSize) + lx * dy * -1;
					targetGridY = (int)floor(logicalPos.Y / gridSize) + ly * dy;
				}

				UnitBase* target = ObjectContainer::getInstance().FindUnitAt(targetGridX, targetGridY);
				if (target && target->GetTag() == ENEMY && target->GetIsAlive()) {
					Attack(*target);
					attacked = true;
				}
			}

			if (attacked) {
				StartMoving({ (float)nextX, (float)nextY }, true);
				return false;
			}
		}
	}

	if (ColliderObject(gridX, gridY, nextX, nextY)) {
		return false;
	}

	StartMoving({ (float)nextX, (float)nextY }, false);
	return true;
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
		if (GetTag() == PLAYER) {
			status.Hp = 0; 
		} else {
			status.IsAlive = false;
			Die();
		}
	}
}

void UnitBase::Die()
{
}
