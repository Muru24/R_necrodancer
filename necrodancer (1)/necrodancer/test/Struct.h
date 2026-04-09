#pragma once
#include "Define.h"

struct Vector2
{
	float X;
	float Y;

	Vector2 operator+(const Vector2& other) const { return { X + other.X, Y + other.Y }; }
	Vector2 operator-(const Vector2& other) const { return { X - other.X, Y - other.Y }; }
	Vector2 operator*(float scalar) const { return { X * scalar, Y * scalar }; }
	bool operator==(const Vector2& other) const { return X == other.X && Y == other.Y; }
};

enum MonsterType
{
	MONSTER_NONE = 0,
	MONSTER_SLIME,
	MONSTER_BAT,
	MONSTER_SKELETON,
	MONSTER_DEAD_RINGER
};

struct UnitStatus
{
	float Hp;
	float MaxHp;
	float Attack;
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


struct MapTile
{
	TileType type;
	int variant;
	int durability;
};

struct AttackEffect
{
	ItemID id;
	Vector2 pos;
	float angle;
	float timer;
	float duration;
	int maxFrames;
};

struct Bomb
{
	Vector2 pos;
	int beatsRemaining = 5;
};

struct ExplosionEffect
{
	Vector2 pos;
	float timer;
	float duration;
	int maxFrames = 8;
};
