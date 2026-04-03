#pragma once
#include "Unit.h"

class Map;

// 플레이어 캐릭터의 이동, 애니메이션, 상태를 관리하는 클래스
class Player : public Unit
{
private:
	int   m_currentFrame;         // 현재 애니메이션 프레임 번호
	unsigned long m_lastAnimTime; // 마지막 프레임 갱신 시간
	bool  m_prevKeyState[4];      // 이전 프레임의 키 눌림 상태 (Edge Detection용)
	Vector2 m_vTargetPos;         // 이동 목표 월드 좌표
	Vector2 m_vStartPos;          // 이동 시작 시점의 월드 좌표
	bool  m_isMoving;             // 현재 이동 중인지 여부
	bool  m_isBumping;            // 벽에 부딪혀 튕겨 나가는 중인지 여부

	int targetGridX, targetGridY; // 이동 목표 맵 타일 좌표
	float moveProgress = 0.0f;    // 이동 진행률 (0.0 ~ 1.0)
	float jumpHeight = 30.0f;     // 이동 시 점프 높이 시각 효과

public:
	Player() : m_currentFrame(0), m_lastAnimTime(0), m_isMoving(false), m_isBumping(false) {
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = false;
		m_vTargetPos = { 0, 0 };
		m_vStartPos = { 0, 0 };
	}
	Player(int hp, int movespeed, Vector2 pos, ObjectTag tag)
		: Unit(hp, movespeed, pos, tag), m_currentFrame(0), m_lastAnimTime(0), m_isMoving(false), m_isBumping(false) {
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = false;
		m_vTargetPos = pos;
		m_vStartPos = pos;
	}

	// 입력에 따른 이동 처리 및 벽 충돌 검사를 수행합니다.
	// - pMap: 충돌 검사용 맵 데이터
	virtual void Move(Map* pMap);

	// 타겟 객체를 공격합니다.
	// - Target: 공격 대상 유닛
	virtual void Attack(Unit& Target);

	// 공격을 받았을 때 데미지를 처리합니다.
	// - atk: 받는 데미지 양
	virtual void TakeDamage(int atk);

	// 사망 시 처리를 수행합니다.
	virtual void Die();

	// 플레이어의 매 프레임 상태(이동 애니메이션 등)를 갱신합니다.
	// - pMap: 월드 데이터
	// - deltaTime: 이전 프레임과의 시간 간격
	virtual void Update(Map* pMap, float deltaTime);

	// 속성 접근자들
	int GetCurrentFrame() const { return m_currentFrame; }
	bool GetIsMoving() const { return m_isMoving; }
	float GetMoveProgress() const { return moveProgress; }
	float GetJumpHeight() const { return jumpHeight; }
};
