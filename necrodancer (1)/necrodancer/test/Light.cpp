#include "Light.h"
#include "Room.h"
#include <cmath>
#include <algorithm>

Light::Light()
{
	Clear();
}

Light::~Light()
{
}

void Light::Update(Vector2 playerPos, int radius, const std::vector<Room*>& rooms)
{
	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {
			if (m_visibility[y][x] == VIS_VISIBLE) {
				m_visibility[y][x] = VIS_EXPLORED;
			}
		}
	}

	int gridSize = FRAME_SIZE * DRAW_SCALE;
	int pGridX = (int)(playerPos.X / gridSize);
	int pGridY = (int)(playerPos.Y / gridSize);

	for (auto* room : rooms) {
		if (room && room->HasRoom()) {
			int rx = room->GetRx();
			int ry = room->GetRy();
			int rw = room->GetRw();
			int rh = room->GetRh();

			if (pGridX >= rx - 1 && pGridX < rx + rw + 1 && pGridY >= ry - 1 && pGridY < ry + rh + 1) {
				for (int y = ry - 1; y <= ry + rh; ++y) {
					for (int x = rx - 1; x <= rx + rw; ++x) {
						if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
							m_visibility[y][x] = VIS_VISIBLE;
						}
					}
				}
			}
		}
	}

	for (int y = pGridY - radius; y <= pGridY + radius; ++y) {
		for (int x = pGridX - radius; x <= pGridX + radius; ++x) {
			if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) continue;

			float dx = (float)(x - pGridX);
			float dy = (float)(y - pGridY);
			float dist = sqrtf(dx * dx + dy * dy);

			if (dist <= (float)radius) {
				m_visibility[y][x] = VIS_VISIBLE;
			}
		}
	}
}

Visibility Light::GetVisibility(int x, int y) const
{
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return VIS_HIDDEN;
	return m_visibility[y][x];
}

void Light::Clear()
{
	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {
			m_visibility[y][x] = VIS_HIDDEN;
		}
	}
}
