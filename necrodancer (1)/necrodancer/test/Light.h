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

	Visibility m_visibility[MAP_HEIGHT][MAP_WIDTH];

public:
	static Light& getInstance()
	{
		static Light instance;
		return instance;
	}

	void Update(Vector2 playerPos, int radius, const std::vector<Room*>& rooms);
	Visibility GetVisibility(int x, int y) const;
	void Clear();
};
