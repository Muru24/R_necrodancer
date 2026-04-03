#include "Player.h"
#include "Define.h"
#include <windows.h>
#include "Map.h"
#include "MainGame.h"
#include <cmath>

void Player::Move()
{
	Map* pMap = MainGame::getInstance().GetMap();
	if (!pMap) return;

	int gridSize = FRAME_SIZE * DRAW_SCALE;
	bool currentKeyState[4];

	currentKeyState[0] = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
	currentKeyState[1] = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
	currentKeyState[2] = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
	currentKeyState[3] = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;

	if (!GetIsMoving()) {
		int dx = 0, dy = 0;

		if (currentKeyState[0] && !m_prevKeyState[0]) dy = -1;
		else if (currentKeyState[1] && !m_prevKeyState[1]) dy = 1;
		else if (currentKeyState[2] && !m_prevKeyState[2]) {
			dx = -1;
			setIsLookLeft(true);
		}
		else if (currentKeyState[3] && !m_prevKeyState[3]) {
			dx = 1;
			setIsLookLeft(false);
		}

		if (dx != 0 || dy != 0) {
			TryMove(dx, dy);
		}
	}

	for (int i = 0; i < 4; ++i) {
		m_prevKeyState[i] = currentKeyState[i];
	}
}

void Player::Attack(UnitBase& Target)
{
	UnitBase::Attack(Target);
}

void Player::TakeDamage(int atk)
{
	UnitBase::TakeDamage(atk);
}

void Player::Die()
{
}

void Player::Update()
{
	UnitBase::Update();
	Move();
}
