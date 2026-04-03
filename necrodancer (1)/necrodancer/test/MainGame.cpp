#include "MainGame.h"
#include "Struct.h"
#include "Define.h"
#include "Map.h"
#include "Player.h"
#include "Render.h"
#include "Camera.h"
#include "Timer.h"
#include "ObjectContainer.h"
#include "Sliem.h"

#include <iostream>

MainGame::MainGame() : m_pPlayer(nullptr), m_pMap(nullptr), m_pCamera(nullptr), m_pTimer(nullptr), m_fDeltaTime(0.0f), m_gdiplusToken(0)
{
}

MainGame::~MainGame()
{
}

void MainGame::Initialize()
{
	Render::getInstance().Initialize(m_gdiplusToken);

	m_pMap = new Map();
	m_pMap->Generate();

	m_pCamera = new Camera();
	m_pTimer = new Timer();
	
	Vector2 startPos = m_pMap->GetRandomFloorPos();
	m_pPlayer = new Player(10, 2, MOVE_SPEED, startPos, PLAYER);
	ObjectContainer::getInstance().PushUnit(m_pPlayer);

	int gridSize = FRAME_SIZE * DRAW_SCALE;

	Vector2 slimePos = { startPos.X + gridSize, startPos.Y };
	Sliem* pSlime = new Sliem(5, 1, 0, slimePos, ENEMY);
	ObjectContainer::getInstance().PushUnit(pSlime);
}

void MainGame::Finalize()
{
	auto& unitContainer = ObjectContainer::getInstance().GetUnitContainer();
	for (auto* unit : unitContainer) {
		if (unit) delete unit;
	}

	m_pPlayer = nullptr;

	if (m_pMap)    { delete m_pMap;    m_pMap    = nullptr; }
	if (m_pCamera) { delete m_pCamera; m_pCamera = nullptr; }
	if (m_pTimer)  { delete m_pTimer;  m_pTimer  = nullptr; }

	Render::getInstance().Finalize(m_gdiplusToken);
}

void MainGame::Update(HWND hWnd)
{
	if (m_pTimer) {
		m_fDeltaTime = m_pTimer->GetDeltaTime();
	}

	auto& unitContainer = ObjectContainer::getInstance().GetUnitContainer();
	for (auto* unit : unitContainer) {
		if (unit && unit->GetIsAlive()) {
			unit->Update();
		}
	}
	
	if (m_pPlayer)
	{
		if (m_pCamera)
		{
			m_pCamera->Update(m_pPlayer->GetPos());
		}
	}
	
	Render::getInstance().DrawUpdate(hWnd, *m_pPlayer);
}

void MainGame::Render(HWND hWnd)
{
	if (m_pMap && m_pCamera)
	{
		Render::getInstance().Draw(hWnd, *m_pMap, *m_pCamera, *m_pPlayer);
	}
}
