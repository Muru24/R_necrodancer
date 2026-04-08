#include "UnitBase.h"
#include "MainGame.h"
#include "Define.h"
#include "Map.h"
#include "ObjectContainer.h"
#include "Player.h"
#include <cmath>
#include <iostream>
#include "Item.h"
#include "ItemFactory.h"
#include "RhythmManager.h"

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

	// 애니메이션 처리: 박자 비율(ratio)에 따라 4프레임을 나눔
	float ratio = RhythmManager::getInstance().GetRatio();
	SetCurrentFrame((int)(ratio * 4) % 4);
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
			if (targetTag == ENEMY || targetTag == NPC) {
				if (targetTag == ENEMY) Attack(*target);
				StartMoving({ (float)nextX, (float)nextY }, true);
				return true;
			}
		}
		else if (myTag == ENEMY) {
			if (targetTag == PLAYER || targetTag == ENEMY || targetTag == NPC) {
				if (targetTag == PLAYER) Attack(*target);
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
	int nextX = (int)floor(logicalPos.X + 0.5f) + dx * gridSize;
	int nextY = (int)floor(logicalPos.Y + 0.5f) + dy * gridSize;

	int gridX = (int)floor((float)nextX / gridSize);
	int gridY = (int)floor((float)nextY / gridSize);

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
				int curGridX = (int)floor((float)logicalPos.X / gridSize);
				int curGridY = (int)floor((float)logicalPos.Y / gridSize);

				if (dx != 0) { // 가로 이동
					targetGridX = curGridX + (int)ly * dx;
					targetGridY = curGridY + (int)lx; // 가로 이동 시 ly가 전진, lx가 측면 옵셋
				}
				else { // 세로 이동
					targetGridX = curGridX + (int)lx; // 세로 이동 시 ly가 전진, lx가 측면 옵셋
					targetGridY = curGridY + (int)ly * dy;
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

void UnitBase::TakeDamage(float atk)
{
	float finalDmg = atk - GetProtection();
	if (finalDmg < 0.5f) finalDmg = 0.5f;

	status.Hp -= finalDmg;
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
	// 몬스터(ENEMY)일 경우에만 골드 드랍
	if (GetTag() == ENEMY) {
		Map* pMap = MainGame::getInstance().GetMap();
		if (pMap) {
			int gridSize = FRAME_SIZE * DRAW_SCALE;
			int gridX = static_cast<int>(GetPos().X / gridSize);
			int gridY = static_cast<int>(GetPos().Y / gridSize);

			ItemBase* pGold = ItemFactory::Create(ITEM_GOLD_COIN);
			if (pGold) {
				pMap->AddWorldItem(pGold, gridX, gridY);
			}
		}
	}
}
