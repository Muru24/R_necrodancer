#pragma once
#include "Struct.h"
#include "Collider.h"

// 게임 내 모든 생명체(플레이어, 몬스터 등)의 기본이 되는 추상 클래스
class Unit
{
private:
	Object obj;        // 태그 정보
	Status status;      // 능력치 및 상태 정보
	Collider collider;  // 충돌체
	bool isLookLeft;    // 좌측 주시 여부

public:
	Unit() {}
	// 유닛을 초기화합니다.
	// - hp, movedistance: 체력 및 이동 거리
	// - pos: 초기 위치
	// - tag: 오브젝트 태그
	Unit(int hp, int movedistance, Vector2 pos, ObjectTag tag)
		: isLookLeft(false)
	{
		status.Hp = hp;
		status.MoveDistance = movedistance;
		status.Position = pos;
		status.IsAlive = true;
		obj.Tag = tag;
	}
	virtual ~Unit() {}

	// 가상 메서드: 하위 클래스에서 각자의 동작을 정의합니다.
	virtual void Move();

	// 대상 유닛을 공격합니다.
	// - Target: 공격 대상
	virtual void Attack(Unit& Target);

	// 데미지를 입고 생존 상태를 업데이트합니다.
	// - atk: 입는 데미지 수치
	virtual void TakeDamage(int atk);

	// 체력이 0 이하가 되었을 때의 처리를 정의합니다.
	virtual void Die();

	// 매 프레임 유닛의 상태를 갱신합니다.
	virtual void Update() {}

	// 게터(Getter) 메서드들
	const int GetHp() const { return status.Hp; }
	const int GetAttack() const { return status.Attack; }
	const int GetDistance() const { return status.MoveDistance; }
	const float GetX() const { return status.Position.X; }
	const float GetY() const { return status.Position.Y; }
	const bool GetIsAlive() const { return status.IsAlive; }
	const bool GetIsLookLeft() const { return isLookLeft; }
	const Vector2 GetPos() const { return status.Position; }
	const ObjectTag GetTag() const { return obj.Tag; }

	// 세터(Setter) 메서드들
	void SetHP(int hp) { status.Hp = hp; }
	void SetAttack(int attack) { status.Attack = attack; }
	void SetDistance(int distance) { status.MoveDistance = distance; }
	void SetX(float x) { status.Position.X = x; }
	void SetY(float y) { status.Position.Y = y; }
	void SetIsAlive(bool isalive) { status.IsAlive = isalive; }
	void setIsLookLeft(bool islookleft) { isLookLeft = islookleft; }
	void SetPos(Vector2 pos) { status.Position = pos; }
	void SetTag(ObjectTag tag) { obj.Tag = tag; }
};
