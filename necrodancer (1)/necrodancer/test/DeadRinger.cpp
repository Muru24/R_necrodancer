#include "DeadRinger.h"
#include "Define.h"
#include "MainGame.h"
#include "Map.h"
#include "Player.h"
#include "UtilPathFinder.h"
#include "RhythmManager.h"
#include "DeadRingerPhantom.h"
#include "ObjectContainer.h"
#include <iostream>
#include <cmath>

DeadRinger::DeadRinger(float hp, float attack, int movedistance, Vector2 pos, ObjectTag tag)
	: UnitBase(hp, attack, movedistance, pos, tag, MONSTER_DEAD_RINGER), m_spawnPos(pos), m_state(DeadRingerState::NORMAL), m_dashDir({ 0,0 })
{
	m_isPhase2 = false;
	m_isInvincible = false;
	m_explosionCount = 0;
	m_phase2Timer = 0;
	m_phantomTimer = 0;
}

void DeadRinger::Move()
{
	if (GetIsMoving()) return;

	Player* pPlayer = MainGame::getInstance().GetPlayer();
	Map* pMap = MainGame::getInstance().GetMap();
	if (!pPlayer || !pMap) return;

	int gridSize = FRAME_SIZE * DRAW_SCALE;
	Vector2 myGridPos = { floor(GetLogicalPos().X / gridSize), floor(GetLogicalPos().Y / gridSize) };
	Vector2 targetGridPos = { floor(pPlayer->GetLogicalPos().X / gridSize), floor(pPlayer->GetLogicalPos().Y / gridSize) };

	if (!m_isPhase2 && status.Hp <= 2.2f) {
		m_isPhase2 = true;
		m_isInvincible = true;
		m_state = DeadRingerState::PHASE2_IDLE;
		m_explosionCount = 0;
		m_phase2Timer = 1 + rand() % 4; 
		m_phantomTimer = 3;

		SetPos(m_spawnPos);
		m_vTargetPos = m_spawnPos;
		SetIsMoving(false);
		return;
	}

	switch (m_state) {
	case DeadRingerState::NORMAL: {
		if (status.Hp <= 4.2f && (rand() % 100 < 10)) {
			m_state = DeadRingerState::READY;
			int dx = (int)(targetGridPos.X - myGridPos.X);
			int dy = (int)(targetGridPos.Y - myGridPos.Y);

			if (abs(dx) > abs(dy)) {
				m_dashDir = { (float)(dx > 0 ? 1 : -1), 0 };
			} else {
				m_dashDir = { 0, (float)(dy > 0 ? 1 : -1) };
			}

			m_skillPath.clear();
			Vector2 testPos = myGridPos;
			while (true) {
				testPos.X += m_dashDir.X;
				testPos.Y += m_dashDir.Y;
				if (testPos.X < 0 || testPos.X >= MAP_WIDTH || testPos.Y < 0 || testPos.Y >= MAP_HEIGHT) break;
				
				TileType t = pMap->GetTile((int)testPos.X, (int)testPos.Y).type;
				TileType tTop = pMap->GetTile((int)testPos.X, (int)testPos.Y + 1).type;
				bool isWall = (t == TILE_WALL_DEFAULT || t == TILE_WALL_HARD || t == TILE_WALL_BADROCK || t == TILE_WALL_SHOP);
				bool isWallTop = (tTop == TILE_WALL_DEFAULT || tTop == TILE_WALL_HARD || tTop == TILE_WALL_BADROCK || tTop == TILE_WALL_SHOP);
				if (isWall || isWallTop) break;

				m_skillPath.push_back(testPos);
			}

			return; 
		}

		Vector2 nextStep = UtilPathFinder::FindNextStepAStar(myGridPos, targetGridPos);
		int adx = (int)(nextStep.X - myGridPos.X);
		int ady = (int)(nextStep.Y - myGridPos.Y);

		if (adx != 0 || ady != 0) {
			if (adx < 0) setIsLookLeft(true);
			else if (adx > 0) setIsLookLeft(false);
			TryMove(adx, ady);
		}
		break;
	}
	case DeadRingerState::READY: {
		if (m_dashDir.X < 0) setIsLookLeft(true);
		else if (m_dashDir.X > 0) setIsLookLeft(false);

		m_state = DeadRingerState::DASH;
		break;
	}
	case DeadRingerState::DASH: {
		Vector2 currentGrid = myGridPos;
		Vector2 finalGrid = myGridPos;

		bool hitPlayer = false;
		while (true) {
			Vector2 nextGrid = { currentGrid.X + m_dashDir.X, currentGrid.Y + m_dashDir.Y };
			
			TileType t = pMap->GetTile((int)nextGrid.X, (int)nextGrid.Y).type;
			TileType tTop = pMap->GetTile((int)nextGrid.X, (int)nextGrid.Y + 1).type;
			bool isWall = (t == TILE_WALL_DEFAULT || t == TILE_WALL_HARD || t == TILE_WALL_BADROCK || t == TILE_WALL_SHOP);
			bool isWallTop = (tTop == TILE_WALL_DEFAULT || tTop == TILE_WALL_HARD || tTop == TILE_WALL_BADROCK || tTop == TILE_WALL_SHOP);
			
			if (isWall || isWallTop) break;

			if ((int)nextGrid.X == (int)targetGridPos.X && (int)nextGrid.Y == (int)targetGridPos.Y) {
				hitPlayer = true;
				break; 
			}

			finalGrid = nextGrid;
			currentGrid = nextGrid;

			if (currentGrid.X < 0 || currentGrid.X >= MAP_WIDTH || currentGrid.Y < 0 || currentGrid.Y >= MAP_HEIGHT) break;
		}

		if (hitPlayer) {
			Attack(*pPlayer);
		}

		if ((int)finalGrid.X != (int)myGridPos.X || (int)finalGrid.Y != (int)myGridPos.Y) {
			Vector2 targetWorldPos = { finalGrid.X * gridSize, finalGrid.Y * gridSize };
			StartMoving(targetWorldPos, false);
		}

		m_state = DeadRingerState::DASHING; 
		break;
	}
	case DeadRingerState::PHASE2_IDLE: {
		m_phase2Timer--;
		if (m_phase2Timer <= 0) {
			m_state = DeadRingerState::SKILL2_READY;
			m_skillPath.clear();
			

			for (int i = 0; i < 3; ++i) {
				int rx = 1 + rand() % 8;   
				int ry = 51 + rand() % 8;  
				
				for (int dy = 0; dy < 4; ++dy) {
					for (int dx = 0; dx < 4; ++dx) {
						m_skillPath.push_back({ (float)rx + dx, (float)ry + dy });
					}
				}
			}
		}
		break;
	}
	case DeadRingerState::SKILL2_READY: {
		m_state = DeadRingerState::PHASE2_IDLE; 
		m_phase2Timer = 1 + rand() % 4; 

		for (const auto& gridPos : m_skillPath) {
			ExplosionEffect* pExp = new ExplosionEffect();
			pExp->pos = { gridPos.X * gridSize, gridPos.Y * gridSize };
			pExp->timer = 0.0f;
			pExp->duration = 0.4f;
			ObjectContainer::getInstance().AddExplosionEffect(pExp);

			UnitBase* pTarget = ObjectContainer::getInstance().FindUnitAt((int)gridPos.X, (int)gridPos.Y);
			if (pTarget) pTarget->TakeDamage(1.0f);
		}
		m_skillPath.clear();
		m_explosionCount++;

		if (m_explosionCount >= 4) {
			m_isInvincible = false;
			m_state = DeadRingerState::PHASE2_VULNERABLE;
			m_phase2Timer = 5; 
		}
		break;
	}
	case DeadRingerState::PHASE2_VULNERABLE: {
		m_phase2Timer--;
		if (m_phase2Timer <= 0) {
			m_state = DeadRingerState::PHASE2_IDLE;
			m_isInvincible = true;
			m_explosionCount = 0;
			m_phase2Timer = 2;
		}
		break;
	}
	}
}

void DeadRinger::Attack(UnitBase& Target)
{
	UnitBase::Attack(Target);
}

void DeadRinger::TakeDamage(float atk)
{
	if (m_isInvincible) {
		return;
	}

	UnitBase::TakeDamage(atk);
	
	if (!m_isPhase2) {
		m_state = DeadRingerState::NORMAL;
		ClearSkillPath();
	}

	SetPos(m_spawnPos);
	m_vTargetPos = m_spawnPos;
	SetIsMoving(false);
}

void DeadRinger::Die()
{
	UnitBase::Die();
}

void DeadRinger::Update()
{
	UnitBase::Update();

	if (m_state == DeadRingerState::NORMAL) {
		float ratio = RhythmManager::getInstance().GetRatio();
		SetCurrentFrame((int)(ratio * 4) % 4);
	}
	else if (m_state == DeadRingerState::READY || m_state == DeadRingerState::DASH) {
		SetCurrentFrame(0);
	}
	else if (m_state == DeadRingerState::DASHING) {
		SetCurrentFrame(1);

		if (!GetIsMoving()) {
			m_state = DeadRingerState::NORMAL;
			m_skillPath.clear();
		}
	}
	else if (m_isPhase2) {
		float ratio = RhythmManager::getInstance().GetRatio();
		SetCurrentFrame((int)(ratio * 5) % 5);
	}
}
