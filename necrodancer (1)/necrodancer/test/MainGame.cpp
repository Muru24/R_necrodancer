#include "MainGame.h"
#include "Struct.h"
#include "Define.h"
#include "Map.h"
#include "Player.h"
#include "Render.h"
#include "Camera.h"
#include "Timer.h"

MainGame::MainGame() : m_pPlayer(nullptr), m_pMap(nullptr), m_pCamera(nullptr), m_pTimer(nullptr), m_gdiplusToken(0)
{
}

MainGame::~MainGame()
{
}

// 모든 게임 시스템과 객체(맵, 카메라, 타이머, 플레이어)를 초기화합니다.
void MainGame::Initialize()
{
	Render::getInstance().Initialize(m_gdiplusToken);

	m_pMap = new Map();
	m_pMap->Generate();

	m_pCamera = new Camera();
	m_pTimer = new Timer();
	
	// 안전한 시작 위치(바닥)를 찾아 플레이어를 생성합니다.
	Vector2 startPos = m_pMap->GetRandomFloorPos();
	m_pPlayer = new Player(10, MOVE_SPEED, startPos, PLAYER);
}

// 게임 종료 시 할당된 모든 메모리를 해제합니다.
void MainGame::Finalize()
{
	if (m_pPlayer) { delete m_pPlayer; m_pPlayer = nullptr; }
	if (m_pMap)    { delete m_pMap;    m_pMap    = nullptr; }
	if (m_pCamera) { delete m_pCamera; m_pCamera = nullptr; }
	if (m_pTimer)  { delete m_pTimer;  m_pTimer  = nullptr; }

	Render::getInstance().Finalize(m_gdiplusToken);
}

// 매 프레임 게임 캐릭터와 카메라의 상태를 갱신합니다.
// - hWnd: 입력 감지용 윈도우 핸들
void MainGame::Update(HWND hWnd)
{
	if (m_pPlayer)
	{
		m_pPlayer->Update(m_pMap, m_pTimer->GetDeltaTime());
		
		// 플레이어의 위치를 따라 카메라를 이동시킵니다.
		if (m_pCamera)
		{
			m_pCamera->Update(m_pPlayer->GetPos());
		}
	}
	
	// 윈도우 렌더링 무효화(Redraw) 요청
	Render::getInstance().DrawUpdate(hWnd, *m_pPlayer);
}

// 현재 게임 세계를 화면에 출력합니다.
// - hWnd: 출력 대상 윈도우 핸들
void MainGame::Render(HWND hWnd)
{
	if (m_pPlayer && m_pMap && m_pCamera)
	{
		Render::getInstance().Draw(hWnd, *m_pPlayer, *m_pMap, *m_pCamera);
	}
}
