#pragma once
#include "Struct.h"
#include "Collider.h"

class UnitBase
{
protected:
	Object obj;
	UnitStatus status;
	Collider collider;
	bool isLookLeft;

	int m_currentFrame;
	unsigned long m_lastAnimTime;
	bool m_isMoving;
	float moveProgress = 0.0f;
	float jumpHeight = 30.0f;

	Vector2 m_vTargetPos;
	Vector2 m_vStartPos;
	bool m_isBumping;

public:
	UnitBase() : isLookLeft(false), m_currentFrame(0), m_lastAnimTime(0), m_isMoving(false), moveProgress(0.0f), jumpHeight(30.0f), m_isBumping(false) {
		status.Hp = 0;
		status.MaxHp = 0;
		m_vTargetPos = { 0, 0 };
		m_vStartPos = { 0, 0 };
	}
	UnitBase(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag)
		: isLookLeft(false), m_currentFrame(0), m_lastAnimTime(0), m_isMoving(false), moveProgress(0.0f), jumpHeight(30.0f), m_isBumping(false)
	{
		status.Hp = hp;
		status.MaxHp = hp;
		status.Attack = attack;
		status.MoveDistance = movedistance;
		obj.Position = pos;
		status.IsAlive = true;
		obj.Tag = tag;
		m_vTargetPos = pos;
		m_vStartPos = pos;
	}
	virtual ~UnitBase() {}

	virtual void Move() =0;
	bool TryMove(int dx, int dy);

	virtual void Attack(UnitBase& Target);

	virtual void TakeDamage(int atk);

	virtual void Die();

	virtual void Update();

	const int GetHp() const { return status.Hp; }
	const int GetMaxHp() const { return status.MaxHp; }
	const int GetAttack() const { return status.Attack; }
	const int GetDistance() const { return status.MoveDistance; }
	const float GetX() const { return obj.Position.X; }
	const float GetY() const { return obj.Position.Y; }
	const bool GetIsAlive() const { return status.IsAlive; }
	const bool GetIsLookLeft() const { return isLookLeft; }
	const Vector2 GetPos() const { return obj.Position; }
	const ObjectTag GetTag() const { return obj.Tag; }
	const Vector2 GetLogicalPos() const { return m_isMoving ? m_vTargetPos : obj.Position; }

	int GetCurrentFrame() const { return m_currentFrame; }
	bool GetIsMoving() const { return m_isMoving; }
	float GetMoveProgress() const { return moveProgress; }
	float GetJumpHeight() const { return jumpHeight; }
	unsigned long GetLastAnimTime() const { return m_lastAnimTime; }

	void SetHP(int hp) { status.Hp = hp; }
	void SetAttack(int attack) { status.Attack = attack; }
	void SetDistance(int distance) { status.MoveDistance = distance; }
	void SetX(float x) { obj.Position.X = x; }
	void SetY(float y) { obj.Position.Y = y; }
	void SetIsAlive(bool isalive) { status.IsAlive = isalive; }
	void setIsLookLeft(bool islookleft) { isLookLeft = islookleft; }
	void SetPos(Vector2 pos) { obj.Position = pos; }
	void SetTag(ObjectTag tag) { obj.Tag = tag; }

	void SetCurrentFrame(int frame) { m_currentFrame = frame; }
	void SetIsMoving(bool moving) { m_isMoving = moving; }
	void SetMoveProgress(float progress) { moveProgress = progress; }
	void SetJumpHeight(float height) { jumpHeight = height; }
	void SetLastAnimTime(unsigned long time) { m_lastAnimTime = time; }

	void StartMoving(Vector2 targetPos, bool isBumping);

	bool ColliderObject(int gridX, int gridY, int nextX, int nextY);
};
