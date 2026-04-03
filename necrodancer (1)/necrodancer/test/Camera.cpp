#include "Camera.h"
#include "Define.h"

Camera::Camera()
{
	m_vPos.X = 0;
	m_vPos.Y = 0;
}

Camera::~Camera()
{
}

// 대상의 위치(주로 플레이어)를 화면 중앙에 오도록 카메라 좌표를 갱신합니다.
// - targetPos: 추적할 대상의 월드 좌표
void Camera::Update(Vector2 targetPos)
{
	int gridSize = FRAME_SIZE * DRAW_SCALE;

	// 대상의 중심점 계산
	float playerCenterX = targetPos.X + (float)gridSize / 2.0f;
	float playerCenterY = targetPos.Y + (float)gridSize / 2.0f;

	// 카메라의 좌상단 시작점 계산 (화면 중앙 맞춤)
	m_vPos.X = playerCenterX - (float)(SCREEN_WIDTH / 2);
	m_vPos.Y = playerCenterY - (float)(SCREEN_HEIGHT / 2);

	// 맵 좌측 및 상단 경계 제한
	if (m_vPos.X < 0) m_vPos.X = 0;
	if (m_vPos.Y < 0) m_vPos.Y = 0;

	// 맵 우측 및 하단 경계 제한
	int maxCamX = MAP_WIDTH * gridSize - SCREEN_WIDTH;
	int maxCamY = MAP_HEIGHT * gridSize - SCREEN_HEIGHT;

	if (m_vPos.X > maxCamX) m_vPos.X = (float)maxCamX;
	if (m_vPos.Y > maxCamY) m_vPos.Y = (float)maxCamY;
}
