#pragma once
#include <vector>
#include "Define.h"
#include "Struct.h"
#include "MapTemplate.h"
#include "Room.h"

// BSP 알고리즘 기반의 맵 생성 및 데이터 관리 클래스
class Map
{
public:
	// 맵 데이터 구조 및 변수를 초기화합니다.
	Map();
	// 할당된 모든 방 객체와 루트 트리를 소각합니다.
	~Map();

	// 전체 맵 지형을 난수로 생성하고 방을 배치합니다.
	void Generate();

	// 특정 좌표의 타일 정보를 반환합니다.
	// - x, y: 맵 상의 타일 좌표
	MapTile GetTile(int x, int y) const;

	// 무작위 바닥 타일의 월드 좌표를 반환합니다.
	Vector2 GetRandomFloorPos() const;

private:
	// BSP 알고리즘을 사용하여 영역을 분할합니다.
	// - node: 분할할 부모 노드(방 영역)
	// - count: 현재 분할 횟수
	void Divide(Room* node, int count);

	// 각 노드 정보를 바탕으로 실제 맵 배열에 타일을 채웁니다.
	// - node: 타일을 채울 대상 노드
	void FillMap(Room* node);

	// 생성된 맵 데이터와 방 정보를 모두 초기화합니다.
	void Clear();

private:
	std::vector<std::vector<MapTile>> m_mapData; // 맵의 전체 타일 배열
	std::vector<Room*> m_rooms;                  // 생성된 모든 방 목록
	Room* m_pRoot;                               // BSP 트리의 루트 노드
	int m_currentSplitCount;                     // 현재까지 분할된 횟수
};
