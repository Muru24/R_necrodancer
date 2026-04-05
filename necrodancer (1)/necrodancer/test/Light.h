#pragma once
#include "Define.h"
#include "Struct.h"
#include <vector>

class Room;

class Light
{
private:
	Light();
	~Light();

	VisibilityType m_visibility[MAP_HEIGHT][MAP_WIDTH];

public:
	static Light& getInstance()
	{
		static Light instance;
		return instance;
	}

	void Update(Vector2 playerPos, int radius, const std::vector<Room*>& rooms);
	VisibilityType GetVisibility(int x, int y) const;
	void Clear();
};
