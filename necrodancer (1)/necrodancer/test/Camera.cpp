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

void Camera::Update(Vector2 targetPos)
{
	int gridSize = FRAME_SIZE * DRAW_SCALE;

	float playerCenterX = targetPos.X + (float)gridSize / 2.0f;
	float playerCenterY = targetPos.Y + (float)gridSize / 2.0f;

	m_vPos.X = playerCenterX - (float)(SCREEN_WIDTH / 2);
	m_vPos.Y = playerCenterY - (float)(SCREEN_HEIGHT / 2);

	if (m_vPos.X < 0) m_vPos.X = 0;
	if (m_vPos.Y < 0) m_vPos.Y = 0;

	int maxCamX = MAP_WIDTH * gridSize - SCREEN_WIDTH;
	int maxCamY = MAP_HEIGHT * gridSize - SCREEN_HEIGHT;

	if (m_vPos.X > maxCamX) m_vPos.X = (float)maxCamX;
	if (m_vPos.Y > maxCamY) m_vPos.Y = (float)maxCamY;
}
