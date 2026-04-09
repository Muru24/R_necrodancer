#include "DeadRingerPhantom.h"
#include "Define.h"
#include "MainGame.h"
#include "ObjectContainer.h"
#include "Map.h"
#include "Player.h"
#include <cmath>

DeadRingerPhantom::DeadRingerPhantom(Vector2 pos, Vector2 dashDir)
	: UnitBase(100, 1, 0, pos, ENEMY, MONSTER_DEAD_RINGER), m_dashDir(dashDir), m_hasStartedDash(false)
{

	if (m_dashDir.X < 0) setIsLookLeft(true);
	else if (m_dashDir.X > 0) setIsLookLeft(false);


	int gridSize = FRAME_SIZE * DRAW_SCALE;
	Vector2 gridPos = { floor(pos.X / gridSize), floor(pos.Y / gridSize) };
	Map* pMap = MainGame::getInstance().GetMap();
	if (pMap) {
		Vector2 curr = gridPos;
		while (true) {
			curr.X += m_dashDir.X;
			curr.Y += m_dashDir.Y;
			if (curr.X < 0 || curr.X >= MAP_WIDTH || curr.Y < 0 || curr.Y >= MAP_HEIGHT) break;

			TileType t = pMap->GetTile((int)curr.X, (int)curr.Y).type;
			TileType tTop = pMap->GetTile((int)curr.X, (int)curr.Y + 1).type;
			bool isWall = (t == TILE_WALL_DEFAULT || t == TILE_WALL_HARD || t == TILE_WALL_BADROCK || t == TILE_WALL_SHOP);
			bool isWallTop = (tTop == TILE_WALL_DEFAULT || tTop == TILE_WALL_HARD || tTop == TILE_WALL_BADROCK || tTop == TILE_WALL_SHOP);
			if (isWall || isWallTop) break;

			m_skillPath.push_back(curr);
		}
	}
}

void DeadRingerPhantom::Move()
{
	if (GetIsMoving()) return;

	if (!m_hasStartedDash) {

		Player* pPlayer = MainGame::getInstance().GetPlayer();
		Map* pMap = MainGame::getInstance().GetMap();
		int gridSize = FRAME_SIZE * DRAW_SCALE;
		Vector2 myGridPos = { floor(GetLogicalPos().X / gridSize), floor(GetLogicalPos().Y / gridSize) };
		Vector2 targetGridPos = { floor(pPlayer->GetLogicalPos().X / gridSize), floor(pPlayer->GetLogicalPos().Y / gridSize) };

		Vector2 currentGrid = myGridPos;
		Vector2 finalGrid = myGridPos;

		bool hitPlayer = false;
		while (true) {
			Vector2 nextGrid = { currentGrid.X + m_dashDir.X, currentGrid.Y + m_dashDir.Y };
			if (nextGrid.X < 0 || nextGrid.X >= MAP_WIDTH || nextGrid.Y < 0 || nextGrid.Y >= MAP_HEIGHT) break;

			TileType t = pMap->GetTile((int)nextGrid.X, (int)nextGrid.Y).type;
			TileType tTop = pMap->GetTile((int)nextGrid.X, (int)nextGrid.Y + 1).type;
			bool isWall = (t == TILE_WALL_DEFAULT || t == TILE_WALL_HARD || t == TILE_WALL_BADROCK || t == TILE_WALL_SHOP);
			bool isWallTop = (tTop == TILE_WALL_DEFAULT || tTop == TILE_WALL_HARD || tTop == TILE_WALL_BADROCK || tTop == TILE_WALL_SHOP);
			if (isWall || isWallTop) break;

			UnitBase* pCollidedUnit = ObjectContainer::getInstance().FindUnitAt((int)nextGrid.X, (int)nextGrid.Y);
			if (pCollidedUnit) {
				if (pCollidedUnit == pPlayer) hitPlayer = true;
				break;
			}


			if ((int)nextGrid.X == (int)targetGridPos.X && (int)nextGrid.Y == (int)targetGridPos.Y) {
				hitPlayer = true;
				break;
			}
			finalGrid = nextGrid;
			currentGrid = nextGrid;
		}

		if (hitPlayer) Attack(*pPlayer);

		if ((int)finalGrid.X != (int)myGridPos.X || (int)finalGrid.Y != (int)myGridPos.Y) {
			Vector2 targetWorldPos = { finalGrid.X * gridSize, finalGrid.Y * gridSize };
			StartMoving(targetWorldPos, false);
		}
		m_hasStartedDash = true;
		m_skillPath.clear();
	}
	else {

		Die();
	}
}

void DeadRingerPhantom::Update()
{
	UnitBase::Update();
	SetCurrentFrame(1);


	if (m_hasStartedDash && !GetIsMoving() && GetIsAlive()) {
		Die();
	}
}

void DeadRingerPhantom::TakeDamage(float atk)
{

}

void DeadRingerPhantom::Die()
{
	UnitBase::Die();

}
