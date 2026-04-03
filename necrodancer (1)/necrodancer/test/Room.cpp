#include "Room.h"
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iostream>

using namespace std;

// 방 객체를 초기화하고 분할 영역 및 타입을 설정합니다.
Room::Room(int x, int y, int w, int h, RoomType type)
	: x(x), y(y), w(w), h(h), rx(0), ry(0), rw(0), rh(0), cx(0), cy(0), left(nullptr), right(nullptr), roomtype(type) {
}

// 재귀적으로 자식 노드들을 삭제하여 메모리를 해제합니다.
Room::~Room() {
	if (left) delete left;
	if (right) delete right;
}

// 영역을 무작위로 분할하여 이진 트리 구조를 형성합니다.
// - minSize: 분할 가능한 최소 너비/높이
void Room::Split(int minSize) {
	if (left || right) return;

	bool isHorizontal = (rand() % 2 == 0);

	// 가로/세로 비율이 일정 수준 이상이면 강제로 분할 방향 결정
	if (w > h && (double)w / h >= 1.25) isHorizontal = false;
	else if (h > w && (double)h / w >= 1.25) isHorizontal = true;

	int maxSize = isHorizontal ? h : w;
	if (maxSize < minSize * 2) return;

	int splitPos = (rand() % (maxSize - minSize * 2 + 1)) + minSize;

	if (isHorizontal) {
		left = new Room(x, y, w, splitPos, DEFAULT);
		right = new Room(x, y + splitPos, w, h - splitPos, DEFAULT);
	}
	else {
		left = new Room(x, y, splitPos, h, DEFAULT);
		right = new Room(x + splitPos, y, w - splitPos, h, DEFAULT);
	}
}

// 리프 노드 영역 내에 실제 방의 좌표와 크기를 무작위로 결정합니다.
void Room::CreateRoom() {
	if (left != nullptr || right != nullptr) {
		if (left) left->CreateRoom();
		if (right) right->CreateRoom();
		this->hasRoom = (left && left->hasRoom) || (right && right->hasRoom);
		return;
	}

	if (w < 9 || h < 9) return;

	rw = (rand() % (w - 2 - 5 + 1)) + 5;
	rh = (rand() % (h - 2 - 5 + 1)) + 5;

	if (rw > rh * 1.5) rw = (int)(rh * 1.5);
	if (rh > rw * 1.5) rh = (int)(rw * 1.5);

	rx = x + (rand() % (w - rw - 1)) + 1;
	ry = y + (rand() % (h - rh - 1)) + 1;
	hasRoom = true;
}

// 방의 중심점 좌표를 계산합니다. (통로 연결용)
void Room::UpdateCenter() {
	if (left && right) {
		left->UpdateCenter();
		right->UpdateCenter();

		if (left->hasRoom && right->hasRoom) {
			cx = (left->cx + right->cx) / 2;
			cy = (left->cy + right->cy) / 2;
			this->hasRoom = true;
		}
		else if (left->hasRoom) {
			cx = left->cx;
			cy = left->cy;
			this->hasRoom = true;
		}
		else if (right->hasRoom) {
			cx = right->cx;
			cy = right->cy;
			this->hasRoom = true;
		}
		else {
			this->hasRoom = false;
		}
	}
	else {
		if (hasRoom) {
			cx = rx + rw / 2;
			cy = ry + rh / 2;
		}
	}
}

// 두 자식 노드의 중심점을 연결하는 통로를 생성합니다.
// - mapData: 통로 정보를 기록할 맵 배열
void Room::ConnectRooms(vector<vector<MapTile>>& mapData) {
	if (left) left->ConnectRooms(mapData);
	if (right) right->ConnectRooms(mapData);

	if (left && right && left->hasRoom && right->hasRoom) {
		int x1 = left->cx;
		int y1 = left->cy;
		int x2 = right->cx;
		int y2 = right->cy;

		if (rand() % 2 == 0) {
			// 'ㄱ'자 형태로 경로 생성 (가로 먼저)
			for (int x = min(x1, x2); x <= max(x1, x2); x++) {
				for (int dy = 0; dy <= 1; dy++) {
					int ty = y1 + dy;
					if (ty >= 0 && ty < MAP_HEIGHT && x >= 0 && x < MAP_WIDTH) {
						mapData[ty][x] = {TILE_FLOOR, 0};
					}
				}
			}
			for (int y = min(y1, y2); y <= max(y1, y2); y++) {
				if (y >= 0 && y < MAP_HEIGHT && x2 >= 0 && x2 < MAP_WIDTH) {
					mapData[y][x2] = {TILE_FLOOR, 0};
				}
			}
		}
		else {
			// 'ㄱ'자 형태로 경로 생성 (세로 먼저)
			for (int y = min(y1, y2); y <= max(y1, y2); y++) {
				if (y >= 0 && y < MAP_HEIGHT && x1 >= 0 && x1 < MAP_WIDTH) {
					mapData[y][x1] = {TILE_FLOOR, 0};
				}
			}
			for (int x = min(x1, x2); x <= max(x1, x2); x++) {
				for (int dy = 0; dy <= 1; dy++) {
					int ty = y2 + dy;
					if (ty >= 0 && ty < MAP_HEIGHT && x >= 0 && x < MAP_WIDTH) {
						mapData[ty][x] = {TILE_FLOOR, 0};
					}
				}
			}
		}
	}
}

// 생성된 방의 총 개수를 반환합니다.
int Room::GetRoomCount() const {
	if (left == nullptr && right == nullptr) {
		return hasRoom ? 1 : 0;
	}
	int count = 0;
	if (left) count += left->GetRoomCount();
	if (right) count += right->GetRoomCount();
	return count;
}
