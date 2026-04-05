#pragma once
#include <vector>
#include "Define.h"
#include "Struct.h"
#include "MapTemplate.h"
#include "Room.h"

class Map
{
public:
	Map();
	~Map();

	void Generate();

	MapTile GetTile(int x, int y) const;

	Vector2 GetRandomFloorPos() const;
	const std::vector<Room*>& GetRooms() const { return m_rooms; }


private:
	void Divide(Room* node, int count);

	void FillMap(Room* node);

	void Clear();

private:
	std::vector<std::vector<MapTile>> m_mapData;
	std::vector<Room*> m_rooms;
	Room* m_pRoot;
	int m_currentSplitCount;
};
