#include "Player.h"
#include "Define.h"
#include <windows.h>
#include "Map.h"
#include <cmath>

// 사용자 입력을 확인하고 이동 목표 및 충돌 처리를 수행합니다.
// - pMap: 맵 데이터 및 타일 정보
void Player::Move(Map* pMap)
{
	if (!pMap) return;

	int gridSize = FRAME_SIZE * DRAW_SCALE;
	bool currentKeyState[4];

	// 방향키 입력 감지
	currentKeyState[0] = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
	currentKeyState[1] = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
	currentKeyState[2] = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
	currentKeyState[3] = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;

	// 이동 중이 아닐 때만 새로운 입력 처리
	if (!m_isMoving) {
		int dx = 0, dy = 0;

		// 키 눌림 감지 (Edge Detection)
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
			int nextX = GetX() + dx * gridSize;
			int nextY = GetY() + dy * gridSize;

			int gridX = nextX / gridSize;
			int gridY = nextY / gridSize;

			// 이동할 위치의 타일 속성 확인
			TileType t = pMap->GetTile(gridX, gridY).type;
			TileType tTop = pMap->GetTile(gridX, gridY + 1).type;
			bool isWall = (t == TILE_WALL_DEFULT || t == TILE_WALL_HARD || t == TILE_WALL_BADROCK || t == TILE_WALL_SHOP);
			bool isWallTop = (tTop == TILE_WALL_DEFULT || tTop == TILE_WALL_HARD || tTop == TILE_WALL_BADROCK || tTop == TILE_WALL_SHOP);

			if (!isWall && !isWallTop) {
				// 이동 가능: 목표 좌표 설정
				m_vStartPos = GetPos();
				m_vTargetPos.X = (float)nextX;
				m_vTargetPos.Y = (float)nextY;
				m_isMoving = true;
				m_isBumping = false;
			}
			else {
				// 이동 불가: 벽에 부딪히는 액션 설정
				m_vStartPos = GetPos();
				m_vTargetPos.X = (float)nextX;
				m_vTargetPos.Y = (float)nextY;
				m_isMoving = true;
				m_isBumping = true;
			}
		}
	}

	for (int i = 0; i < 4; ++i) {
		m_prevKeyState[i] = currentKeyState[i];
	}
}

// 대상을 공격합니다.
// - Target: 공격을 받을 대상 유닛
void Player::Attack(Unit& Target)
{
	Unit::Attack(Target);
}

// 데미지를 입었을 때의 처리를 수행합니다. (현재 미구현)
// - atk: 입는 데미지 수치
void Player::TakeDamage(int atk)
{
}

// 플레이어 사망 시의 처리를 수행합니다. (현재 미구현)
void Player::Die()
{
}

// 플레이어의 이동 애니메이션 및 상태를 매 프레임 업데이트합니다.
// - pMap: 월드 데이터
// - deltaTime: 프레임 간 경과 시간
void Player::Update(Map* pMap, float deltaTime)
{
	if (m_isMoving) {
		int speed = MOVE_SPEED;
		if (m_isBumping) speed *= 2; // 부딪힐 때의 연출 속도 증가

		moveProgress += speed * deltaTime;

		if (moveProgress >= 1.0f)
		{
			moveProgress = 1.0f;
			if (m_isBumping) {
				SetPos(m_vStartPos); // 원래 위치로 복귀
			} else {
				SetPos(m_vTargetPos); // 목표 위치로 이동 완료
			}
			m_isMoving = false;
			m_isBumping = false;
			moveProgress = .0f;
		}
		else
		{
			Vector2 pos;
			if (m_isBumping) {
				// 벽 충돌 연출: 사인 곡선을 이용해 살짝 튕겨 나옴
				float bumpFactor = sinf(moveProgress * 3.141592f) * 0.3f;
				pos.X = m_vStartPos.X + (m_vTargetPos.X - m_vStartPos.X) * bumpFactor;
				pos.Y = m_vStartPos.Y + (m_vTargetPos.Y - m_vStartPos.Y) * bumpFactor;
			} else {
				// 정상 이동: 선형 보간
				pos.X = m_vStartPos.X + (m_vTargetPos.X - m_vStartPos.X) * moveProgress;
				pos.Y = m_vStartPos.Y + (m_vTargetPos.Y - m_vStartPos.Y) * moveProgress;
			}
			SetPos(pos);
		}
	}

	Move(pMap);

	// 애니메이션 프레임 순환
	if (GetTickCount() - m_lastAnimTime >= ANIM_SPEED) {
		m_currentFrame = (m_currentFrame + 1) % 4; 
		m_lastAnimTime = GetTickCount();
	}
}
