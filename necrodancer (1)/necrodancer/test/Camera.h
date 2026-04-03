#pragma once
#include "Struct.h"

// 플레이어를 추적하며 화면 출력 범위를 결정하는 카메라 클래스
class Camera
{
public:
	Camera();
	~Camera();

	// 대상의 위치를 중심으로 카메라 좌표를 갱신합니다. (맵 경계 제한 포함)
	// - targetPos: 카메라가 중심에 두고 자 하는 대상의 월드 좌표
	void Update(Vector2 targetPos);

	// 속성 접근자들
	float GetX() const { return m_vPos.X; }
	float GetY() const { return m_vPos.Y; }
	Vector2 GetPos() const { return m_vPos; }

private:
	Vector2 m_vPos; // 카메라의 현재 좌측 상단 월드 좌표
};
