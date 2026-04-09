#include "Map.h"
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <utility>
#include "Render.h"

using namespace std;

Map::Map() : m_currentSplitCount(0), m_pRoot(nullptr) {
	m_mapData.resize(MAP_HEIGHT, vector<MapTile>(MAP_WIDTH, {TILE_WALL_DEFAULT, 0, 1}));
}

Map::~Map() {
	Clear();
	ClearWorldItems();
}

void Map::Generate() {
	Clear();
	srand((unsigned int)time(NULL));

	m_pRoot = new Room(0, 0, 60, 40, DEFAULT);
	Divide(m_pRoot, 4);
	m_pRoot->CreateRoom();
	FillMap(m_pRoot);
	
	if (m_rooms.size() >= 3) {
		int startIdx = rand() % m_rooms.size();
		m_rooms[startIdx]->SetRoomType(START);

		int bossIdx = -1;
		float maxDistToStart = -1.0f;
		int startCx = m_rooms[startIdx]->GetRx() + m_rooms[startIdx]->GetRw() / 2;
		int startCy = m_rooms[startIdx]->GetRy() + m_rooms[startIdx]->GetRh() / 2;

		for (int i = 0; i < (int)m_rooms.size(); ++i) {
			if (i == startIdx) continue;
			int currCx = m_rooms[i]->GetRx() + m_rooms[i]->GetRw() / 2;
			int currCy = m_rooms[i]->GetRy() + m_rooms[i]->GetRh() / 2;
			float dist = (float)sqrt(pow(currCx - startCx, 2) + pow(currCy - startCy, 2));
			if (dist > maxDistToStart) {
				maxDistToStart = dist;
				bossIdx = i;
			}
		}

		if (bossIdx != -1) {
			m_rooms[bossIdx]->SetRoomType(BOSS);
			int bossRh = (int)m_BossMapData.size();
			int bossRw = (bossRh > 0) ? (int)m_BossMapData[0].size() : 0;
			
			for (int y = m_rooms[bossIdx]->GetRy(); y < m_rooms[bossIdx]->GetRy() + m_rooms[bossIdx]->GetRh(); ++y) {
				for (int x = m_rooms[bossIdx]->GetRx(); x < m_rooms[bossIdx]->GetRx() + m_rooms[bossIdx]->GetRw(); ++x) {
					if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) m_mapData[y][x] = {TILE_WALL_DEFAULT, 0};
				}
			}

			m_rooms[bossIdx]->SetRw(bossRw);
			m_rooms[bossIdx]->SetRh(bossRh);
			
			for (int y = m_rooms[bossIdx]->GetRy(); y < m_rooms[bossIdx]->GetRy() + m_rooms[bossIdx]->GetRh(); ++y) {
				for (int x = m_rooms[bossIdx]->GetRx(); x < m_rooms[bossIdx]->GetRx() + m_rooms[bossIdx]->GetRw(); ++x) {
					if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) m_mapData[y][x] = {TILE_FLOOR, 0};
				}
			}

			int shopIdx = -1;
			float maxDistToBoss = -1.0f;
			int bossCx = m_rooms[bossIdx]->GetRx() + m_rooms[bossIdx]->GetRw() / 2;
			int bossCy = m_rooms[bossIdx]->GetRy() + m_rooms[bossIdx]->GetRh() / 2;

			for (int i = 0; i < (int)m_rooms.size(); ++i) {
				if (i == startIdx || i == bossIdx) continue;
				int currCx = m_rooms[i]->GetRx() + m_rooms[i]->GetRw() / 2;
				int currCy = m_rooms[i]->GetRy() + m_rooms[i]->GetRh() / 2;
				float dist = (float)sqrt(pow(currCx - bossCx, 2) + pow(currCy - bossCy, 2));
				if (dist > maxDistToBoss) {
					maxDistToBoss = dist;
					shopIdx = i;
				}
			}

			if (shopIdx != -1) {
				m_rooms[shopIdx]->SetRoomType(SHOP);
				int shopRh = (int)m_ShopMapData.size();
				int shopRw = (shopRh > 0) ? (int)m_ShopMapData[0].size() : 0;

				for (int y = m_rooms[shopIdx]->GetRy(); y < m_rooms[shopIdx]->GetRy() + m_rooms[shopIdx]->GetRh(); ++y) {
					for (int x = m_rooms[shopIdx]->GetRx(); x < m_rooms[shopIdx]->GetRx() + m_rooms[shopIdx]->GetRw(); ++x) {
						if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) m_mapData[y][x] = {TILE_WALL_DEFAULT, 0};
					}
				}

				m_rooms[shopIdx]->SetRw(shopRw);
				m_rooms[shopIdx]->SetRh(shopRh);
				
				for (int y = m_rooms[shopIdx]->GetRy(); y < m_rooms[shopIdx]->GetRy() + m_rooms[shopIdx]->GetRh(); ++y) {
					for (int x = m_rooms[shopIdx]->GetRx(); x < m_rooms[shopIdx]->GetRx() + m_rooms[shopIdx]->GetRw(); ++x) {
						if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) m_mapData[y][x] = {TILE_FLOOR, 0};
					}
				}
			}
		}
	}

	m_pRoot->UpdateCenter();
	m_pRoot->ConnectRooms(m_mapData);

	for (auto* room : m_rooms) {
		if (room->GetRoomType() == BOSS) {
			int bossRh = (int)m_BossMapData.size();
			int bossRw = (bossRh > 0) ? (int)m_BossMapData[0].size() : 0;
			int bRx = room->GetRx();
			int bRy = room->GetRy();
			for (int y = 0; y < bossRh; ++y) {
				for (int x = 0; x < bossRw; ++x) {
					if (bRy + y < MAP_HEIGHT && bRx + x < MAP_WIDTH) {
						m_mapData[bRy + y][bRx + x] = {(TileType)m_BossMapData[y][x], 0};
					}
				}
			}

			int centerX = bRx + bossRw / 2;
			int centerY = bRy + bossRh / 2;
			if (centerY < MAP_HEIGHT && centerX < MAP_WIDTH) {
				m_mapData[centerY][centerX] = { TILE_BOSS_SPECIAL_FLOOR, 0 };
			}
		} 
		else if (room->GetRoomType() == SHOP) {
			int shopRh = (int)m_ShopMapData.size();
			int shopRw = (shopRh > 0) ? (int)m_ShopMapData[0].size() : 0;
			int sRx = room->GetRx();
			int sRy = room->GetRy();

			for (int y = 0; y < shopRh; ++y) {
				for (int x = 0; x < shopRw; ++x) {
					if (sRy + y < MAP_HEIGHT && sRx + x < MAP_WIDTH) {
						m_mapData[sRy + y][sRx + x] = {(TileType)m_ShopMapData[y][x], 0};
					}
				}
			}

			for (int y = sRy - 1; y <= sRy + shopRh; ++y) {
				for (int x = sRx - 1; x <= sRx + shopRw; ++x) {
					if (x >= sRx && x < sRx + shopRw && y >= sRy && y < sRy + shopRh) continue;
					if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
						m_mapData[y][x] = {TILE_WALL_SHOP, 0, 2};
					}
				}
			}

			int doorX = sRx + shopRw / 2;
			int doorY = sRy + shopRh; 
			if (doorY < MAP_HEIGHT) {
				m_mapData[doorY][doorX] = {TILE_FLOOR, 0};

				int targetY = -1;
				for (int ty = doorY + 1; ty < MAP_HEIGHT; ++ty) {
					if (m_mapData[ty][doorX].type == TILE_FLOOR) {
						targetY = ty;
						break;
					}
				}

				if (targetY != -1) {
					for (int ty = doorY + 1; ty < targetY; ++ty) {
						m_mapData[ty][doorX] = {TILE_FLOOR, 0};
					}
				} else {
					for (int ty = doorY + 1; ty < min(doorY + 4, MAP_HEIGHT); ++ty) {
						m_mapData[ty][doorX] = {TILE_FLOOR, 0};
					}
				}
			}
		}
	}


	std::vector<int> dist(MAP_WIDTH * MAP_HEIGHT, 9999);
	std::vector<std::pair<int, int>> queue;

	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {
			if (m_mapData[y][x].type == TILE_FLOOR) {
				dist[y * MAP_WIDTH + x] = 0;
				queue.push_back({ x, y });
			}
		}
	}

	int head = 0;
	int dx[8] = {-1, 1, 0, 0, -1, 1, -1, 1};
	int dy[8] = {0, 0, -1, 1, -1, -1, 1, 1};

	while (head < (int)queue.size()) {
		int cx = queue[head].first;
		int cy = queue[head].second;
		head++;

		for (int i = 0; i < 8; ++i) {
			int nx = cx + dx[i];
			int ny = cy + dy[i];
			if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT) {
				int nextDist = dist[cy * MAP_WIDTH + cx] + 1;
				if (nextDist <= 2 && dist[ny * MAP_WIDTH + nx] > nextDist) {
					dist[ny * MAP_WIDTH + nx] = nextDist;
					queue.push_back({ nx, ny });
				}
			}
		}
	}

	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {
			if (m_mapData[y][x].type == TILE_WALL_DEFAULT) {
				int d = dist[y * MAP_WIDTH + x];
				if (d == 1) { 
					m_mapData[y][x].type = TILE_WALL_DEFAULT;
					m_mapData[y][x].variant = rand() % (WALL_DEFAULT_RANGE + 1);
					m_mapData[y][x].durability = 1;
				}
				else if (d == 2) { 
					m_mapData[y][x].type = TILE_WALL_HARD;
					m_mapData[y][x].variant = 0; 
					m_mapData[y][x].durability = 2;
				}
				else { 
					m_mapData[y][x].type = TILE_WALL_BADROCK;
					m_mapData[y][x].variant = 0; 
					m_mapData[y][x].durability = 999;
				}
			}
		}
	}
	

	int secretBossStartX = 0;
	int secretBossStartY = 50;
	int secretBossSize = 13;
	for (int y = secretBossStartY; y < secretBossStartY + secretBossSize; ++y) {
		for (int x = secretBossStartX; x < secretBossStartX + secretBossSize; ++x) {
			if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
				if (y == secretBossStartY || y == secretBossStartY + secretBossSize - 1 ||
					x == secretBossStartX || x == secretBossStartX + secretBossSize - 1) {
					m_mapData[y][x] = { TILE_WALL_HARD, 0, 2 };
				}
				else {
					m_mapData[y][x] = { TILE_FLOOR, 0 };
				}
			}
		}
	}


	Room* secretRoom = new Room(secretBossStartX, secretBossStartY, secretBossSize, secretBossSize, BOSS);
	secretRoom->SetRx(secretBossStartX + 1);
	secretRoom->SetRy(secretBossStartY + 1);
	secretRoom->SetRw(secretBossSize - 2);
	secretRoom->SetRh(secretBossSize - 2);
	secretRoom->SetHasRoom(true);
	m_rooms.push_back(secretRoom);

	Render::getInstance().InvalidateBackgroundCache();
}

Room* Map::GetRoomAt(int x, int y) const
{
	for (Room* room : m_rooms)
	{
		if (x >= room->GetRx() && x < room->GetRx() + room->GetRw() &&
			y >= room->GetRy() && y < room->GetRy() + room->GetRh())
		{
			return room;
		}
	}
	return nullptr;
}
 
void Map::AddWorldItem(ItemBase* pItem, int x, int y)
{
	if (!pItem) return;
	m_worldItems.push_back({ pItem, x, y });
}

ItemBase* Map::PickupItem(int x, int y)
{
	for (auto it = m_worldItems.begin(); it != m_worldItems.end(); ++it)
	{
		if (it->x == x && it->y == y)
		{
			ItemBase* pItem = it->item;
			m_worldItems.erase(it);
			return pItem;
		}
	}
	return nullptr;
}

void Map::ClearWorldItems()
{
	for (auto& wi : m_worldItems)
	{
		if (wi.item)
		{
			delete wi.item;
			wi.item = nullptr;
		}
	}
	m_worldItems.clear();
}

MapTile Map::GetTile(int x, int y) const {
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return {TILE_WALL_DEFAULT, 0, 1};
	return m_mapData[y][x];
}

bool Map::DigTile(int x, int y, int digLevel)
{
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return false;

	MapTile& tile = m_mapData[y][x];

	if (tile.type == TILE_WALL_BADROCK) return false;

	if (tile.type == TILE_WALL_DEFAULT) {
		if (digLevel >= 1) {
			tile.type = TILE_FLOOR;
			Render::getInstance().UpdateTileCache(x, y, this);
			Render::getInstance().UpdateTileCache(x, y - 1, this);
			return true;
		}
	}
	else if (tile.type == TILE_WALL_HARD || tile.type == TILE_WALL_SHOP) {
		if (digLevel >= 3) {
			tile.type = TILE_FLOOR;
			Render::getInstance().UpdateTileCache(x, y, this);
			Render::getInstance().UpdateTileCache(x, y - 1, this);
			return true;
		}
		else if (digLevel == 2) {
			tile.durability--;
			if (tile.durability <= 0) {
				tile.type = TILE_FLOOR;
				Render::getInstance().UpdateTileCache(x, y, this);
				Render::getInstance().UpdateTileCache(x, y - 1, this);
				return true;
			}
		}
	}

	return false;
}

Vector2 Map::GetRandomFloorPos() const {
	int gridSize = FRAME_SIZE * DRAW_SCALE;
	
	for (auto* room : m_rooms) {
		if (room->GetRoomType() == START) {
			float startX = (float)(room->GetRx() + room->GetRw() / 2) * gridSize;
			float startY = (float)(room->GetRy() + room->GetRh() / 2) * gridSize;
			return { startX, startY };
		}
	}

	vector<Vector2> floors;
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			if (m_mapData[y][x].type == TILE_FLOOR) {
				floors.push_back({ (float)x * gridSize, (float)y * gridSize });
			}
		}
	}

	if (floors.empty()) return { 0, 0 };
	return floors[rand() % floors.size()];
}

void Map::Divide(Room* node, int count) {
	const int MAX_ROOMS = 6;
	const int MIN_SIZE = 8;
	if (count <= 0 || m_currentSplitCount >= MAX_ROOMS - 1) return;

	node->Split(MIN_SIZE);
	if (node->GetLeft() != nullptr && node->GetRight() != nullptr) {
		m_currentSplitCount++;
		Divide(node->GetLeft(), count - 1);
		Divide(node->GetRight(), count - 1);
	}
}

void Map::FillMap(Room* node) {
	if (node->GetLeft() != nullptr || node->GetRight() != nullptr) {
		if (node->GetLeft()) FillMap(node->GetLeft());
		if (node->GetRight()) FillMap(node->GetRight());
	}
	else {
		if (node->GetRw() > 0 && node->GetRh() > 0) {
			m_rooms.push_back(node);
			for (int i = node->GetRy(); i < node->GetRy() + node->GetRh(); i++) {
				for (int j = node->GetRx(); j < node->GetRx() + node->GetRw(); j++) {
					if (i >= 0 && i < MAP_HEIGHT && j >= 0 && j < MAP_WIDTH) {
						m_mapData[i][j] = {TILE_FLOOR, 0};
					}
				}
			}
		}
	}
}

void Map::Clear() {
	if (m_pRoot) {
		delete m_pRoot;
		m_pRoot = nullptr;
	}
	m_currentSplitCount = 0;
	m_rooms.clear();
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			m_mapData[i][j] = {TILE_WALL_DEFAULT, 0};
		}
	}
}
