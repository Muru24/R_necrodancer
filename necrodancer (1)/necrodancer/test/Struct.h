#pragma once

struct Vector2
{
	float X;
	float Y;

	Vector2 operator+(const Vector2& other) const { return { X + other.X, Y + other.Y }; }
	Vector2 operator-(const Vector2& other) const { return { X - other.X, Y - other.Y }; }
	Vector2 operator*(float scalar) const { return { X * scalar, Y * scalar }; }
	bool operator==(const Vector2& other) const { return X == other.X && Y == other.Y; }
};

struct UnitStatus
{
	int Hp;
	int MaxHp;
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