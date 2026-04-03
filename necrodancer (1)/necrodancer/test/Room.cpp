#include "Room.h"
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iostream>

using namespace std;

Room::Room(int x, int y, int w, int h, RoomType type)
	: x(x), y(y), w(w), h(h), rx(0), ry(0), rw(0), rh(0), cx(0), cy(0), left(nullptr), right(nullptr), roomtype(type) {
}

Room::~Room() {
	if (left) delete left;
	if (right) delete right;
}

void Room::Split(int minSize) {
	if (left || right) return;

	bool isHorizontal = (rand() % 2 == 0);

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

void Room::ConnectRooms(vector<vector<MapTile>>& mapData) {
	if (left) left->ConnectRooms(mapData);
	if (right) right->ConnectRooms(mapData);

	if (left && right && left->hasRoom && right->hasRoom) {
		int x1 = left->cx;
		int y1 = left->cy;
		int x2 = right->cx;
		int y2 = right->cy;

		if (rand() % 2 == 0) {
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

int Room::GetRoomCount() const {
	if (left == nullptr && right == nullptr) {
		return hasRoom ? 1 : 0;
	}
	int count = 0;
	if (left) count += left->GetRoomCount();
	if (right) count += right->GetRoomCount();
	return count;
}
