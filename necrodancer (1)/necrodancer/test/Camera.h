#pragma once
#include "Struct.h"

class Camera
{
public:
	Camera();
	~Camera();

	void Update(Vector2 targetPos);

	float GetX() const { return m_vPos.X; }
	float GetY() const { return m_vPos.Y; }
	Vector2 GetPos() const { return m_vPos; }

private:
	Vector2 m_vPos;
};
