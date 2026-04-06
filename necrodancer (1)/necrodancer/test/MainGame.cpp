#include "MainGame.h"
#include "Struct.h"
#include "Define.h"
#include "Map.h"
#include "Player.h"
#include "Render.h"
#include "Camera.h"
#include "Timer.h"
#include "ObjectContainer.h"
#include "Slime.h"
#include "Light.h"
#include "ItemFactory.h"
#include "RhythmManager.h"
#include "Shopkeeper.h"

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
	Light::getInstance().Clear();

	m_pCamera = new Camera();
	m_pTimer = new Timer();
	m_Rhytm = &RhythmManager::getInstance();
	m_Rhytm->SetNoteList(RHYTHM_BPM);

	Vector2 startPos = m_pMap->GetRandomFloorPos();
	m_pPlayer = new Player(10, 2, MOVE_SPEED, startPos, PLAYER);
	ObjectContainer::getInstance().PushUnit(m_pPlayer);

	m_pPlayer->Equip(ItemFactory::Create(ITEM_SHOVEL_GOLD));
	m_pPlayer->Equip(ItemFactory::Create(ITEM_LONGSWORD));

	int gridSize = FRAME_SIZE * DRAW_SCALE;

	Vector2 slimePos = { startPos.X + gridSize, startPos.Y };
	Slime* pSlime = new Slime(5, 1, 0, slimePos, ENEMY);
	ObjectContainer::getInstance().PushUnit(pSlime);

	const std::vector<Room*>& rooms = m_pMap->GetRooms();
	Room* pShop = nullptr;
	for (Room* room : rooms) {
		if (room->GetRoomType() == SHOP) {
			pShop = room;
			break;
		}
	}
	if (pShop) {
		int cx = pShop->GetRx() + pShop->GetRw() / 2;
		int cy = pShop->GetRy() + pShop->GetRh() / 2;
		Vector2 shopkeeperPos = { (float)cx * gridSize + gridSize, (float)cy * gridSize };
		Shopkeeper* pShopkeeper = new Shopkeeper(999, 0, 0, shopkeeperPos, NPC);
		ObjectContainer::getInstance().PushUnit(pShopkeeper);
	}

	beatInterval = 60000 / RHYTHM_BPM;
	lastBeatTime = GetTickCount();
	currentBeatCount = 0;
}

void MainGame::Finalize()
{
	auto& unitContainer = ObjectContainer::getInstance().GetUnitContainer();
	for (auto* unit : unitContainer) {
		if (unit) delete unit;
	}

	m_pPlayer = nullptr;

	if (m_pMap) { delete m_pMap;    m_pMap = nullptr; }
	if (m_pCamera) { delete m_pCamera; m_pCamera = nullptr; }
	if (m_pTimer) { delete m_pTimer;  m_pTimer = nullptr; }
	if (m_Rhytm) { m_Rhytm->FinishRhythm(); m_Rhytm = nullptr; }
	Render::getInstance().Finalize(m_gdiplusToken);
}

void MainGame::Update(HWND hWnd)
{
	if (m_pTimer) {
		m_fDeltaTime = m_pTimer->GetDeltaTime();
	}

	bool isBeat = false;
	if (m_Rhytm)
	{
		isBeat = m_Rhytm->UpdateRhythm();
	}

	auto& unitContainer = ObjectContainer::getInstance().GetUnitContainer();
	for (auto* unit : unitContainer) {
		if (unit && unit->GetIsAlive()) {
			unit->Update();

			if (isBeat && unit->GetTag() == ENEMY) {
				unit->Move();
			}
		}
	}

	if (m_pPlayer)
	{
		if (m_pMap)
		{
			Light::getInstance().Update(m_pPlayer->GetPos(), m_pPlayer->GetVisionRadius(), m_pMap->GetRooms());
		}

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
