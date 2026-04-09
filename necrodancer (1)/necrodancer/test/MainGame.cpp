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
#include "Bat.h"
#include "Skeleton.h"
#include "Light.h"
#include "ItemFactory.h"
#include "RhythmManager.h"
#include "Shopkeeper.h"
#include "Title.h"
#include "DeadRinger.h"

#include <iostream>
#include <set>
#include <ctime>

MainGame::MainGame() : m_pPlayer(nullptr), m_pMap(nullptr), m_pCamera(nullptr), m_pTimer(nullptr), m_fDeltaTime(0.0f), m_gdiplusToken(0), m_pTitle(nullptr), m_eScene(SCENE_TITLE)
{
}

MainGame::~MainGame()
{
}

void MainGame::Initialize()
{
	if (m_gdiplusToken == 0)
		Render::getInstance().Initialize(m_gdiplusToken);

	Finalize(); 

	m_eScene = SCENE_TITLE;
	m_pTitle = new Title();
	m_pTitle->Initialize();

	m_pMap = new Map();
	m_pMap->Generate();
	Light::getInstance().Clear();
	srand((unsigned int)time(NULL));

	m_pCamera = new Camera();
	m_pTimer = new Timer();
	m_Rhytm = &RhythmManager::getInstance();
	m_Rhytm->SetNoteList(RHYTHM_BPM);

	Vector2 startPos = m_pMap->GetRandomFloorPos();
	m_pPlayer = new Player(3, 0, MOVE_SPEED, startPos, PLAYER);
	ObjectContainer::getInstance().PushUnit(m_pPlayer);	

	m_pPlayer->Equip(ItemFactory::Create(ITEM_DAGGER));
	m_pPlayer->Equip(ItemFactory::Create(ITEM_SHOVEL_IRON));
	m_pPlayer->Equip(ItemFactory::Create(ITEM_CHEESE));

	int gridSize = FRAME_SIZE * DRAW_SCALE;
	int px = static_cast<int>(startPos.X / gridSize);
	int py = static_cast<int>(startPos.Y / gridSize);
	ItemBase* pTestBomb = ItemFactory::Create(ITEM_BOMB);
	if (pTestBomb) {
		m_pMap->AddWorldItem(pTestBomb, px + 1, py);
	}

	const std::vector<Room*>& rooms = m_pMap->GetRooms();
	for (Room* room : rooms) {
		RoomType type = room->GetRoomType();
		if (type == SHOP || type == BOSS || type == START) continue;

		int rw = room->GetRw();
		int rh = room->GetRh();
		int tileCount = rw * rh;
		int slimeCount = 0;

		if (tileCount < 25) slimeCount = 2;
		else if (tileCount < 37) slimeCount = 3;
		else slimeCount = 4;

		std::set<std::pair<int, int>> spawnedPos;
		for (int i = 0; i < slimeCount; ++i) {
			int tx, ty;
			int attempts = 0;
			bool success = false;
			do {
				if (rw >= 3 && rh >= 3) {
					tx = room->GetRx() + 1 + (rand() % (rw - 2));
					ty = room->GetRy() + 1 + (rand() % (rh - 2));
				}
				else {
					tx = room->GetRx() + (rand() % rw);
					ty = room->GetRy() + (rh > 0 ? (rand() % rh) : 0);
				}
				attempts++;

				if (spawnedPos.count({ tx, ty }) == 0 && 
					m_pMap->GetTile(tx, ty).type == TILE_FLOOR &&
					ObjectContainer::getInstance().FindUnitAt(tx, ty) == nullptr) {
					success = true;
					break;
				}
			} while (attempts < 100);

			if (!success) continue;

			spawnedPos.insert({ tx, ty });
			Vector2 monsterPos = { (float)tx * gridSize, (float)ty * gridSize };

			UnitBase* pMonster = nullptr;
			int dice = rand() % 3;
			if (dice == 0) {
				pMonster = new Slime(2, 1, 0, monsterPos, ENEMY);
			}
			else if (dice == 1) {
				pMonster = new Bat(1, 1, 0, monsterPos, ENEMY);
			}
			else {
				pMonster = new Skeleton(3, 2, 0, monsterPos, ENEMY);
			}

			if (pMonster) {
				ObjectContainer::getInstance().PushUnit(pMonster);
			}
		}
	}

	m_pPlayer->SetMoney(20);

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
		Vector2 shopkeeperPos = { (float)cx * gridSize, (float)cy * gridSize };
		Shopkeeper* pShopkeeper = new Shopkeeper(999, 0, 0, shopkeeperPos, NPC);
		ObjectContainer::getInstance().PushUnit(pShopkeeper);


		ItemID shopItems[] = { ITEM_DAGGER, ITEM_LONGSWORD, ITEM_RAPIER };
		for (int i = 0; i < 3; ++i) {
			int r = i + rand() % (3 - i);
			std::swap(shopItems[i], shopItems[r]);
		}

		for (int i = 0; i < 3; ++i) {
			int itemX = cx - 1 + i;
			int itemY = cy + 1;
			ItemBase* pItem = ItemFactory::Create(shopItems[i]);
			if (pItem) {
				pItem->SetPrice(10);
				m_pMap->AddWorldItem(pItem, itemX, itemY);
			}
		}
	}
	beatInterval = 60000 / RHYTHM_BPM;
	lastBeatTime = GetTickCount();
	currentBeatCount = 0;


	int brCenterX = 6;
	int brTopY = 51;
	gridSize = FRAME_SIZE * DRAW_SCALE;
	Vector2 deadRingerPos = { (float)brCenterX * gridSize, (float)brTopY * gridSize };
	DeadRinger* pDeadRinger = new DeadRinger(6, 2, 0, deadRingerPos, ENEMY);
	if (pDeadRinger) {
		ObjectContainer::getInstance().PushUnit(pDeadRinger);
	}


	ObjectContainer::getInstance().ProcessSpawnQueue();
}

void MainGame::Finalize()
{
	ObjectContainer::getInstance().Clear();

	m_pPlayer = nullptr;

	if (m_pMap) { delete m_pMap;    m_pMap = nullptr; }
	if (m_pCamera) { delete m_pCamera; m_pCamera = nullptr; }
	if (m_pTimer) { delete m_pTimer;  m_pTimer = nullptr; }
	if (m_Rhytm) { m_Rhytm->FinishRhythm(); m_Rhytm = nullptr; }
	if (m_pTitle) { delete m_pTitle;  m_pTitle = nullptr; }
}

void MainGame::Release()
{
	Finalize();
	Render::getInstance().Finalize(m_gdiplusToken);
}

void MainGame::Update(HWND hWnd)
{
	if (m_eScene == SCENE_TITLE)
	{
		if (m_pTitle) m_pTitle->Update();
		Render::getInstance().DrawUpdate(hWnd, *m_pPlayer);
		return;
	}

	if (m_pTimer) {
		m_fDeltaTime = m_pTimer->GetDeltaTime();
	}

	ObjectContainer::getInstance().UpdateEffects(m_fDeltaTime);
	ObjectContainer::getInstance().UpdateExplosions(m_fDeltaTime);

	bool isBeat = false;
	if (m_Rhytm)
	{
		isBeat = m_Rhytm->UpdateRhythm();
		if (isBeat) {
			ObjectContainer::getInstance().UpdateBombs();
		}
		if (isBeat && m_pPlayer) {
			int prevCombo = m_pPlayer->GetComboCount();


			if (!m_pPlayer->GetActedThisBeat()) {
				m_pPlayer->SetComboCount(0);
			}


			int currentCombo = m_pPlayer->GetComboCount();
			if (currentCombo >= 1 || (prevCombo > 0 && currentCombo == 0)) {
				Render::getInstance().ToggleAniPhase();
			}

			m_pPlayer->ResetActedThisBeat();
		}
	}

	auto& unitContainer = ObjectContainer::getInstance().GetUnitContainer();

	Room* pPlayerRoom = nullptr;
	if (m_pPlayer && m_pMap) {
		int gridSize = FRAME_SIZE * DRAW_SCALE;
		int px = (int)(m_pPlayer->GetLogicalPos().X / gridSize);
		int py = (int)(m_pPlayer->GetLogicalPos().Y / gridSize);
		pPlayerRoom = m_pMap->GetRoomAt(px, py);
	}

	for (auto* unit : unitContainer) {
		if (unit && unit->GetIsAlive()) {
			unit->Update();

			if (isBeat && unit->GetTag() == ENEMY) {
				if (pPlayerRoom && m_pMap) {
					int gridSize = FRAME_SIZE * DRAW_SCALE;
					int mx = (int)(unit->GetLogicalPos().X / gridSize);
					int my = (int)(unit->GetLogicalPos().Y / gridSize);
					Room* pMonsterRoom = m_pMap->GetRoomAt(mx, my);

					if (pPlayerRoom == pMonsterRoom && pPlayerRoom != nullptr) {
						unit->Move();
					}
				}
			}
		}
	}


	ObjectContainer::getInstance().ProcessSpawnQueue();

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
	if (m_eScene == SCENE_TITLE)
	{
		if (m_pTitle)
			Render::getInstance().DrawTitle(hWnd, *m_pTitle);
		return;
	}

	if (m_pMap && m_pCamera)
	{
		Render::getInstance().Draw(hWnd, *m_pMap, *m_pCamera, *m_pPlayer);
	}
}
