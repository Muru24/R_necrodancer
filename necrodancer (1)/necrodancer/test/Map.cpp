#include "Map.h"
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <utility>
#include "Render.h"

using namespace std;

// 맵 클래스 생성자: 기본 맵 크기를 설정하고 초기화합니다.
Map::Map() : m_currentSplitCount(0), m_pRoot(nullptr) {
	m_mapData.resize(MAP_HEIGHT, vector<MapTile>(MAP_WIDTH, {TILE_WALL_DEFULT, 0}));
}

// 맵 클래스 소멸자: 할당된 자원을 해제합니다.
Map::~Map() {
	Clear();
}

// 절차적 생성 알고리즘을 통해 맵 전체 지형을 구성합니다.
void Map::Generate() {
	Clear();
	srand((unsigned int)time(NULL));

	// 루트 노드 생성 및 BSP 분할
	m_pRoot = new Room(0, 0, MAP_WIDTH, MAP_HEIGHT, DEFAULT);
	Divide(m_pRoot, 7);
	m_pRoot->CreateRoom();
	FillMap(m_pRoot);
	
	if (m_rooms.size() >= 3) {
		// 시작 지점(START) 설정
		int startIdx = rand() % m_rooms.size();
		m_rooms[startIdx]->SetRoomType(START);

		// 시작 지점에서 가장 먼 방을 보스 방(BOSS)으로 설정
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
			
			// 보스 방 영역 초기화 및 설정
			for (int y = m_rooms[bossIdx]->GetRy(); y < m_rooms[bossIdx]->GetRy() + m_rooms[bossIdx]->GetRh(); ++y) {
				for (int x = m_rooms[bossIdx]->GetRx(); x < m_rooms[bossIdx]->GetRx() + m_rooms[bossIdx]->GetRw(); ++x) {
					if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) m_mapData[y][x] = {TILE_WALL_DEFULT, 0};
				}
			}

			m_rooms[bossIdx]->SetRw(bossRw);
			m_rooms[bossIdx]->SetRh(bossRh);
			
			for (int y = m_rooms[bossIdx]->GetRy(); y < m_rooms[bossIdx]->GetRy() + m_rooms[bossIdx]->GetRh(); ++y) {
				for (int x = m_rooms[bossIdx]->GetRx(); x < m_rooms[bossIdx]->GetRx() + m_rooms[bossIdx]->GetRw(); ++x) {
					if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) m_mapData[y][x] = {TILE_FLOOR, 0};
				}
			}

			// 보스 방에서 가장 먼 방을 상점(SHOP)으로 설정
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
						if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) m_mapData[y][x] = {TILE_WALL_DEFULT, 0};
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

	// 방 사이 통로 연결
	m_pRoot->UpdateCenter();
	m_pRoot->ConnectRooms(m_mapData);

	// 특수 방(보스/상점) 템플릿 타일링 및 테두리 설정
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

			// 상점 전전용 테두리 벽 배치
			for (int y = sRy - 1; y <= sRy + shopRh; ++y) {
				for (int x = sRx - 1; x <= sRx + shopRw; ++x) {
					if (x >= sRx && x < sRx + shopRw && y >= sRy && y < sRy + shopRh) continue;
					if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
						if (m_mapData[y][x].type == TILE_WALL_DEFULT) {
							m_mapData[y][x] = {TILE_WALL_SHOP, 0};
						}
					}
				}
			}
		}
	}

	// BFS를 통한 벽 티어(Tier) 계산
	std::vector<std::vector<int>> dist(MAP_HEIGHT, std::vector<int>(MAP_WIDTH, 9999));
	std::vector<std::pair<int, int>> queue;

	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {
			if (m_mapData[y][x].type == TILE_FLOOR) {
				dist[y][x] = 0;
				queue.push_back({x, y});
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
				if (dist[ny][nx] > dist[cy][cx] + 1) {
					dist[ny][nx] = dist[cy][cx] + 1;
					queue.push_back({nx, ny});
				}
			}
		}
	}

	// 거리에 따른 벽 유형 및 랜덤 변형값 할당
	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {
			if (m_mapData[y][x].type == TILE_WALL_DEFULT) {
				int d = dist[y][x];
				if (d == 1) { 
					m_mapData[y][x].type = TILE_WALL_DEFULT;
					m_mapData[y][x].variant = rand() % (WALL_DEFAULT_RANGE + 1);
				}
				else if (d == 2) { 
					m_mapData[y][x].type = TILE_WALL_HARD;
					m_mapData[y][x].variant = 0; 
				}
				else { 
					m_mapData[y][x].type = TILE_WALL_BADROCK;
					m_mapData[y][x].variant = 0; 
				}
			}
		}
	}

	cout << "Total Rooms Created: " << m_pRoot->GetRoomCount() << endl;

	// 콘솔 출력용 맵 시각화
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			if (m_mapData[y][x].type == TILE_WALL_DEFULT) cout << "1";
			else if (m_mapData[y][x].type == TILE_WALL_SHOP) cout << "#"; 
			else {
				char tileChar = '0';
				for (auto* room : m_rooms) {
					if (x >= room->GetRx() && x < room->GetRx() + room->GetRw() &&
						y >= room->GetRy() && y < room->GetRy() + room->GetRh()) {
						switch (room->GetRoomType()) {
						case START: tileChar = 's'; break;
						case SHOP:  tileChar = 'h'; break;
						case BOSS:  tileChar = 'b'; break;
						}
						break;
					}
				}
				cout << tileChar;
			}
		}
		cout << endl;
	}

	// 생성 결과 요약 출력
	cout << "\n--- Individual Room Info ---" << endl;
	for (size_t i = 0; i < m_rooms.size(); ++i) {
		cout << "Room #" << (i + 1) << " | Pos: (" << m_rooms[i]->GetRx() << ", " << m_rooms[i]->GetRy() 
			 << ") | Size: " << m_rooms[i]->GetRw() << "x" << m_rooms[i]->GetRh() << " TYPE : "<<m_rooms[i]->GetRoomType()<< endl;
	}
	cout << "----------------------------\n" << endl;

	// 배경 렌더링 캐시 무효화
	Render::getInstance().InvalidateBackgroundCache();
}

// 특정 좌표의 타일 정보를 가져옵니다.
// - x, y: 맵 상의 좌표 (0 ~ MAP_WIDTH-1, 0 ~ MAP_HEIGHT-1)
MapTile Map::GetTile(int x, int y) const {
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return {TILE_WALL_DEFULT, 0};
	return m_mapData[y][x];
}

// 시작 지점 혹은 임의의 바닥 좌표를 반환합니다.
Vector2 Map::GetRandomFloorPos() const {
	int gridSize = FRAME_SIZE * DRAW_SCALE;
	
	// 우선적으로 시작 방의 중앙 위치를 반환
	for (auto* room : m_rooms) {
		if (room->GetRoomType() == START) {
			float startX = (float)(room->GetRx() + room->GetRw() / 2) * gridSize;
			float startY = (float)(room->GetRy() + room->GetRh() / 2) * gridSize;
			return { startX, startY };
		}
	}

	// 시작 방이 없는 경우 모든 바닥 타일 중 무작위 반환
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

// 재귀적으로 영역을 분할하여 BSP 트리 노드를 생성합니다.
void Map::Divide(Room* node, int count) {
	const int MAX_ROOMS = 20;
	const int MIN_SIZE = 8;
	if (count <= 0 || m_currentSplitCount >= MAX_ROOMS - 1) return;

	node->Split(MIN_SIZE);
	if (node->GetLeft() != nullptr && node->GetRight() != nullptr) {
		m_currentSplitCount++;
		Divide(node->GetLeft(), count - 1);
		Divide(node->GetRight(), count - 1);
	}
}

// BSP 트리의 리프 노드를 실제 맵 데이터(TILE_FLOOR)로 채웁니다.
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

// 모든 맵 데이터를 공백 또는 벽으로 초기화합니다.
void Map::Clear() {
	if (m_pRoot) {
		delete m_pRoot;
		m_pRoot = nullptr;
	}
	m_currentSplitCount = 0;
	m_rooms.clear();
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			m_mapData[i][j] = {TILE_WALL_DEFULT, 0};
		}
	}
}
