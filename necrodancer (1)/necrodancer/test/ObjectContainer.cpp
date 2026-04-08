#include "ObjectContainer.h"
#include <iostream>
#include <cmath>

void ObjectContainer::PushUnit(UnitBase* unit)
{
	UnitContainer.push_back(unit);
}

void ObjectContainer::PopUnit(int pos)
{
	UnitContainer.erase(UnitContainer.begin()+pos);
}

void ObjectContainer::Clear()
{
	for (auto* unit : UnitContainer)
	{
		if (unit) delete unit;
	}
	UnitContainer.clear();

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
}
