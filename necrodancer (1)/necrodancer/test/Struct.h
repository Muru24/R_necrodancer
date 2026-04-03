#pragma once

struct Vector2
{
	float X;
	float Y;
};

struct UnitStatus
{
	int Hp;
	int Attack;
	int MoveDistance;
	bool IsAlive;
};

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

struct Object
{
	ObjectTag Tag;
	Vector2 Position;
};

#include "Define.h"

struct MapTile
{
	TileType type;
	int variant;
};