#pragma once
#include <vector>
#include "Struct.h"
#include "Define.h"

// 방의 용도를 구분하는 열거형
enum RoomType
{
	DEFAULT, // 기본 방
	START,   // 시작 지점
	SHOP,    // 상점
	BOSS,    // 보스 방
	EVENT    // 이벤트 방
};

// BSP 트리의 노드이자 개별 방 영역을 관리하는 클래스
class Room {
private:
	int x, y, w, h;       // 분할된 전체 영역의 좌표 및 크기
	int rx, ry, rw, rh;   // 실제 방이 배치된 영역의 좌표 및 크기
	int cx, cy;           // 방의 중심점 좌표
	bool hasRoom = false; // 실제 방 생성 여부
	Room* left, * right;  // 자식 노드 포인터
	RoomType roomtype;    // 방의 종류

public:
	// 새로운 노드를 생성합니다.
	// - x, y, w, h: 분할 영역 정보
	// - type: 초기 방 종류
	Room(int x, int y, int w, int h, RoomType type);
	~Room();

	// 영역을 좌우 또는 상하로 무작위 분할합니다.
	// - minSize: 분할 가능한 최소 크기
	void Split(int minSize);

	// 분할된 리프 노드 영역 내에 실제 방을 생성합니다.
	void CreateRoom();

	// 자식 노드들의 정보를 바탕으로 중심점 좌표를 갱신합니다.
	void UpdateCenter();

	// 자식 노드(방) 간의 통로를 맵 데이터에 그립니다.
	// - mapData: 통로를 그릴 대상 맵 배열
	void ConnectRooms(std::vector<std::vector<MapTile>>& mapData);

	// 현재 노드를 포함한 모든 하위 노드의 방 개수를 반환합니다.
	int GetRoomCount() const;

	// 게터(Getter) 및 세터(Setter) 메서드들
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
};
