#pragma once
#include <vector>
#include "Define.h"
#include "Struct.h"
#include "MapTemplate.h"
#include "Room.h"
#include "Item.h"

struct WorldItem {
	ItemBase* item;
	int x, y;
};

class Map
{
public:
	Map();
	~Map();

	void Generate();

	MapTile GetTile(int x, int y) const;

	Vector2 GetRandomFloorPos() const;
	bool DigTile(int x, int y, int digLevel);
	const std::vector<Room*>& GetRooms() const { return m_rooms; }

	void AddWorldItem(ItemBase* pItem, int x, int y);
	ItemBase* PickupItem(int x, int y);
	void ClearWorldItems();
	const std::vector<WorldItem>& GetWorldItems() const { return m_worldItems; }


private:
	void Divide(Room* node, int count);

	void FillMap(Room* node);

	void Clear();

private:
	std::vector<std::vector<MapTile>> m_mapData;
	std::vector<Room*> m_rooms;
	Room* m_pRoot;
	int m_currentSplitCount;
	std::vector<WorldItem> m_worldItems;
};
