#pragma once
#include <vector>
#include "Struct.h"
#include "Define.h"

enum RoomType
{
	DEFAULT,
	START,
	SHOP,
	BOSS,
	EVENT
};

class Room {
private:
	int x, y, w, h;
	int rx, ry, rw, rh;
	int cx, cy;
	bool hasRoom = false;
	Room* left, * right;
	RoomType roomtype;

public:
	Room(int x, int y, int w, int h, RoomType type);
	~Room();

	void Split(int minSize);

	void CreateRoom();

	void UpdateCenter();

	void ConnectRooms(std::vector<std::vector<MapTile>>& mapData);

	int GetRoomCount() const;

	int GetX() const { return x; }
	int GetY() const { return y; }
	int GetW() const { return w; }
	int GetH() const { return h; }
	int GetRx() const { return rx; }
	int GetRy() const { return ry; }
	int GetRw() const { return rw; }
	int GetRh() const { return rh; }
	bool HasRoom() const { return hasRoom; }

	Room* GetLeft() const { return left; }
	Room* GetRight() const { return right; }

	RoomType GetRoomType() const { return roomtype; }
	void SetRoomType(RoomType type) { roomtype = type; }

	void SetRx(int _rx) { rx = _rx; }
	void SetRy(int _ry) { ry = _ry; }
	void SetRw(int _rw) { rw = _rw; }
	void SetRh(int _rh) { rh = _rh; }
	void SetHasRoom(bool _hasRoom) { hasRoom = _hasRoom; }
};
