#include <iostream>
#include <cmath>
#include <algorithm>
#include "Map.h"
#include "MainGame.h"
#include "Render.h"
#include "ObjectContainer.h"

void ObjectContainer::PushUnit(UnitBase* unit)
{
	m_spawnQueue.push_back(unit);
}

void ObjectContainer::PopUnit(int pos)
{
	UnitContainer.erase(UnitContainer.begin()+pos);
}

void ObjectContainer::ProcessSpawnQueue()
{
	if (m_spawnQueue.empty()) return;

	for (auto* unit : m_spawnQueue) {
		UnitContainer.push_back(unit);
	}
	m_spawnQueue.clear();
}

void ObjectContainer::Clear()
{
	for (auto* unit : UnitContainer)
	{
		if (unit) delete unit;
	}
	UnitContainer.clear();

	for (auto* unit : m_spawnQueue)
	{
		if (unit) delete unit;
	}
	m_spawnQueue.clear();

	ClearEffects();
}

UnitBase* ObjectContainer::FindUnitAt(int gridX, int gridY)
{
	int gridSize = FRAME_SIZE * DRAW_SCALE;
	for (auto* unit : UnitContainer)
	{
		if (!unit || !unit->GetIsAlive()) continue;

		Vector2 logicalPos = unit->GetLogicalPos();
		int uGridX = (int)floor(((float)logicalPos.X + gridSize * 0.5f) / gridSize);
		int uGridY = (int)floor(((float)logicalPos.Y + gridSize * 0.5f) / gridSize);

		if (uGridX == gridX && uGridY == gridY)
		{
			return unit;
		}


		if (unit->GetIsMoving()) {
			Vector2 startPos = unit->GetStartPos();
			int sGridX = (int)floor(((float)startPos.X + gridSize * 0.5f) / gridSize);
			int sGridY = (int)floor(((float)startPos.Y + gridSize * 0.5f) / gridSize);

			if (sGridX == gridX && sGridY == gridY) {
				return unit;
			}
		}
	}
	return nullptr;
}

void ObjectContainer::AddAttackEffect(ItemID id, Vector2 pos, float angle)
{
	AttackEffect* effect = new AttackEffect();
	effect->id = id;
	effect->pos = pos;
	effect->angle = angle;
	effect->timer = 0.0f;
	effect->duration = 0.2f;
	effect->maxFrames = (id == ITEM_RAPIER) ? 4 : 3;
	m_attackEffects.push_back(effect);
}

void ObjectContainer::UpdateEffects(float deltaTime)
{
	for (auto it = m_attackEffects.begin(); it != m_attackEffects.end(); )
	{
		(*it)->timer += deltaTime;
		if ((*it)->timer >= (*it)->duration)
		{
			delete* it;
			it = m_attackEffects.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ObjectContainer::ClearEffects()
{
	for (auto* effect : m_attackEffects)
	{
		if (effect) delete effect;
	}
	m_attackEffects.clear();

	for (auto* bomb : m_bombs)
	{
		if (bomb) delete bomb;
	}
	m_bombs.clear();

	for (auto* exp : m_explosions)
	{
		if (exp) delete exp;
	}
	m_explosions.clear();
}

void ObjectContainer::AddBomb(Vector2 pos)
{
	Bomb* pBomb = new Bomb();
	pBomb->pos = pos;
	pBomb->beatsRemaining = 5;
	m_bombs.push_back(pBomb);
}

void ObjectContainer::UpdateBombs()
{
	for (auto it = m_bombs.begin(); it != m_bombs.end(); )
	{
		(*it)->beatsRemaining--;
		if ((*it)->beatsRemaining <= 0)
		{
			Explode(*it);
			delete* it;
			it = m_bombs.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ObjectContainer::Explode(Bomb* pBomb)
{
	Map* pMap = MainGame::getInstance().GetMap();
	if (!pMap) return;

	int gridSize = FRAME_SIZE * DRAW_SCALE;

	int centerX = (int)floor(pBomb->pos.X / gridSize + 0.5f);
	int centerY = (int)floor(pBomb->pos.Y / gridSize + 0.5f);


	for (int dy = -1; dy <= 2; ++dy) {
		for (int dx = -1; dx <= 1; ++dx) {
			int tx = centerX + dx;
			int ty = centerY + dy;


			pMap->DigTile(tx, ty, 999);

			for (auto* unit : UnitContainer) {
				if (!unit || !unit->GetIsAlive()) continue;
				Vector2 uPos = unit->GetLogicalPos();
				int ux = (int)floor(uPos.X / gridSize + 0.5f);
				int uy = (int)floor(uPos.Y / gridSize + 0.5f);

				if (ux == tx && uy == ty) {
					unit->TakeDamage(2.0f);
				}
			}


			if (dy <= 1) {
				ExplosionEffect* pExp = new ExplosionEffect();
				pExp->pos = { (float)tx * gridSize, (float)ty * gridSize };
				pExp->timer = 0.0f;
				pExp->duration = 0.4f;
				m_explosions.push_back(pExp);
			}
		}
	}
}

void ObjectContainer::AddExplosionEffect(ExplosionEffect* pExp)
{
	if (pExp) m_explosions.push_back(pExp);
}

void ObjectContainer::UpdateExplosions(float deltaTime)
{
	for (auto it = m_explosions.begin(); it != m_explosions.end(); )
	{
		(*it)->timer += deltaTime;
		if ((*it)->timer >= (*it)->duration)
		{
			delete* it;
			it = m_explosions.erase(it);
		}
		else
		{
			++it;
		}
	}
}
