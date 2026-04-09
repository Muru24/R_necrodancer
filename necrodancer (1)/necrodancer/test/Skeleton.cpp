#include "Skeleton.h"
#include "MainGame.h"
#include "Map.h"
#include "Player.h"
#include <cmath>
#include "RhythmManager.h"
#include "UtilPathFinder.h"

Skeleton::Skeleton(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag)
	: UnitBase(hp, attack, movedistance, pos, tag, MONSTER_SKELETON), m_beatCount(0)
{
}

void Skeleton::Move()
{
	if (GetIsMoving()) return;

	m_beatCount = (m_beatCount + 1) % 2;


	if (m_beatCount != 0) return;

	Player* pPlayer = MainGame::getInstance().GetPlayer();
	Map* pMap = MainGame::getInstance().GetMap();
	if (!pPlayer || !pMap) return;

	int gridSize = FRAME_SIZE * DRAW_SCALE;
	Vector2 myGridPos = { floor(GetLogicalPos().X / gridSize), floor(GetLogicalPos().Y / gridSize) };
	Vector2 targetGridPos = { floor(pPlayer->GetLogicalPos().X / gridSize), floor(pPlayer->GetLogicalPos().Y / gridSize) };

	Vector2 nextStep = UtilPathFinder::FindNextStepAStar(myGridPos, targetGridPos);
	
	int dx = (int)(nextStep.X - myGridPos.X);
	int dy = (int)(nextStep.Y - myGridPos.Y);

	if (dx != 0 || dy != 0) {
		TryMove(dx, dy);
	}
}

void Skeleton::Update()
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
			if (m_isBumping) SetPos(m_vStartPos);
			else SetPos(m_vTargetPos);
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


	float ratio = RhythmManager::getInstance().GetRatio();
	int baseFrame = (m_beatCount == 1) ? 0 : 4;
	SetCurrentFrame(baseFrame + (int)(ratio * 4) % 4);
}
void Skeleton::Die()
{
	UnitBase::Die();
}
