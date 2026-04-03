#pragma once

// 2D 위치 좌표 구조체
struct Vector2
{
	float X;
	float Y;
};

// 유닛의 기본 능력치 정보 구조체
struct Status
{
	int Hp;             // 체력
	int Attack;         // 공격력
	int MoveDistance;   // 한 턴당 이동 거리
	bool IsAlive;       // 생존 여부
	Vector2 Position;   // 현재 위치
};

// 오브젝트를 구분하기 위한 태그 열거형
enum ObjectTag
{
	NONE = 0,
	PLAYER,
	ENEMY,
	NPC,
	TRAP,
	CONTAINERS,
	ITEM,
	WALL,
	SHRINES
};

// 게임 내 모든 오브젝트의 공통 속성 구조체
struct Object
{
	ObjectTag Tag;
};

#include "Define.h"

// 맵의 한 칸 정보를 저장하는 구조체
struct MapTile
{
	TileType type;      // 타일 종류 (바닥, 벽 등)
	int variant;        // 타일의 외형 변형값 (랜덤 벽 번호 등)
};