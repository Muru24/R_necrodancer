#include "Player.h"
#include "Define.h"
#include <windows.h>
#include "Map.h"
#include "MainGame.h"
#include <cmath>
#include "Item.h"
#include <iostream>
#include "ObjectContainer.h"
#include "RhythmManager.h"

void Player::Move()
{
	bool currentPlusKey = (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) != 0 || (GetAsyncKeyState(VK_ADD) & 0x8000) != 0;
	if (currentPlusKey && !m_bPrevPlusKey) {
		TeleportToBossCenter();
		m_bPrevPlusKey = currentPlusKey;
		return;
	}
	m_bPrevPlusKey = currentPlusKey;

	if (!RhythmManager::getInstance().IsBeatWindow()) {
		bool keyState[4];
		keyState[0] = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
		keyState[1] = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
		keyState[2] = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
		keyState[3] = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;

		bool input = false;
		for (int i = 0; i < 4; ++i) {
			if (keyState[i] && !m_prevKeyState[i]) input = true;
			m_prevKeyState[i] = keyState[i];
		}

		if (input) m_comboCount = 0;
		return;
	}

	if (m_bActedThisBeat) {
		bool currentKeyState[4];
		currentKeyState[0] = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
		currentKeyState[1] = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
		currentKeyState[2] = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
		currentKeyState[3] = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = currentKeyState[i];
		return;
	}

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

		if (currentKeyState[1] && currentKeyState[2] && (!m_prevKeyState[1] || !m_prevKeyState[2])) {

			UseConsumable();
			m_bActedThisBeat = true;
			AddComboCount(1);
			dx = 0; dy = 0;
		}
		else if (currentKeyState[1] && currentKeyState[3] && (!m_prevKeyState[1] || !m_prevKeyState[3])) {

			if (m_Bombs > 0) {
				m_Bombs--;
				ObjectContainer::getInstance().AddBomb(GetLogicalPos());
				m_bActedThisBeat = true;
				AddComboCount(1);
				dx = 0; dy = 0;
			}
		}
		else if (currentKeyState[0] && !m_prevKeyState[0]) dy = -1;
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
			if (TryMove(dx, dy)) {

				int px = static_cast<int>(m_vTargetPos.X / gridSize);
				int py = static_cast<int>(m_vTargetPos.Y / gridSize);
				ItemBase* pFloorItem = pMap->PickupItem(px, py);
				if (pFloorItem) {
					int price = pFloorItem->GetPrice();
					if (price > GetMoney()) {

						pMap->AddWorldItem(pFloorItem, px, py);
					}
					else {

						if (price > 0) {
							SetMoney(-price);
							pFloorItem->SetPrice(0);
						}
						Equip(pFloorItem);
					}
				}


				if (pMap->GetTile(px, py).type == TILE_BOSS_SPECIAL_FLOOR) {

					float targetX = (float)(0 + 6) * gridSize;
					float targetY = (float)(50 + 6) * gridSize;
					
					obj.Position = { targetX, targetY };
					m_vTargetPos = obj.Position;
					m_vStartPos = obj.Position;
					m_isMoving = false;
					moveProgress = 0.0f;
					
					std::cout << "[Debug] Secret Boss Room Teleport!" << std::endl;
				}
			}
			m_bActedThisBeat = true;
			AddComboCount(1);
		}
	}

	for (int i = 0; i < 4; ++i) {
		m_prevKeyState[i] = currentKeyState[i];
	}
}

void Player::Attack(UnitBase& Target)
{
	float finalDmg = status.Attack;
	

	/*
	if (m_comboCount == 3) {
		finalDmg += 1.0f;
	}
	*/

	ItemBase* pWeapon = GetEquippedItem(SLOT_WEAPON);
	if (pWeapon) {
		finalDmg += pWeapon->GetBaseDamage();
		pWeapon->ApplySpecialAbility(TRIGGER_ON_ATTACK, this, &Target);
	}
	
	Target.TakeDamage(finalDmg);


	float angle = 0.0f;
	Vector2 myPos = GetLogicalPos();
	Vector2 targetPos = Target.GetLogicalPos();
	int gridSize = FRAME_SIZE * DRAW_SCALE;

	int myGridX = (int)(myPos.X / gridSize);
	int myGridY = (int)(myPos.Y / gridSize);
	int targetGridX = (int)(targetPos.X / gridSize);
	int targetGridY = (int)(targetPos.Y / gridSize);

	Vector2 effectPos = myPos;
	if (targetGridX > myGridX) {
		angle = 0.0f;
		effectPos.X += gridSize;
	}
	else if (targetGridY > myGridY) {
		angle = 90.0f;
		effectPos.Y += gridSize;
	}
	else if (targetGridX < myGridX) {
		angle = 180.0f;
		effectPos.X -= gridSize;
	}
	else if (targetGridY < myGridY) {
		angle = 270.0f;
		effectPos.Y -= gridSize;
	}

	ItemID weaponID = ITEM_DAGGER;
	if (pWeapon) weaponID = pWeapon->GetID();

	ObjectContainer::getInstance().AddAttackEffect(weaponID, effectPos, angle);
}

void Player::TakeDamage(float atk)
{
	ItemBase* pArmor = GetEquippedItem(SLOT_BODY);
	if (pArmor) {
		pArmor->ApplySpecialAbility(TRIGGER_ON_DAMAGED, this, nullptr);
	}

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

void Player::Equip(ItemBase* pItem)
{
	if (!pItem) return;

	if (pItem->GetID() == ITEM_GOLD_COIN) {
		SetMoney(1); 
		delete pItem;
		return;
	}
	if (pItem->GetID() == ITEM_BOMB) {
		AddBombs(1);
		delete pItem;
		return;
	}

	Map* pMap = MainGame::getInstance().GetMap();
	int gridSize = FRAME_SIZE * DRAW_SCALE;
	int px = static_cast<int>(GetLogicalPos().X / gridSize);
	int py = static_cast<int>(GetLogicalPos().Y / gridSize);

	for (auto it = m_equips.begin(); it != m_equips.end(); ++it) {
		if (it->first == pItem->GetSlot()) {
			ItemBase* oldItem = it->second;
			oldItem->OnUnequip(this);
			
			if (pMap) {
				pMap->AddWorldItem(oldItem, px, py);
			} else {
				delete oldItem;
			}

			m_equips.erase(it);
			break;
		}
	}

	m_equips.push_back(std::make_pair(pItem->GetSlot(), pItem));
	pItem->OnEquip(this);
}

void Player::DestroyItem(ItemBase* pItem)
{
	if (!pItem) return;
	for (auto it = m_equips.begin(); it != m_equips.end(); ++it) {
		if (it->second == pItem) {
			it->second->OnUnequip(this);


			delete it->second;
			m_equips.erase(it);
			break;
		}
	}
}

ItemBase* Player::GetEquippedItem(ItemSlot slot) const
{
	for (const auto& pair : m_equips) {
		if (pair.first == slot) return pair.second;
	}
	return nullptr;
}

void Player::UseConsumable()
{
	ItemBase* pItem = GetEquippedItem(SLOT_CONSUMABLE);
	if (!pItem) return;


	if (pItem->GetID() == ITEM_CHEESE) {

		status.Hp += 2;
		if (status.Hp > status.MaxHp) status.Hp = status.MaxHp;
		std::cout << "Player used Cheese! HP restored to " << status.Hp << std::endl;
	}


	DestroyItem(pItem);
}

float Player::GetProtection() const
{
	float totalProt = 0.0f;
	ItemBase* pArmor = GetEquippedItem(SLOT_BODY);
	if (pArmor) totalProt += pArmor->GetProtection();
	return totalProt;
}

float Player::GetDigLevel() const
{
	float baseDig = 1.0f;
	ItemBase* pShovel = GetEquippedItem(SLOT_SHOVEL);
	if (pShovel) return pShovel->GetDigStrength();
	return baseDig;
}

void Player::TeleportToBossCenter()
{
	Map* pMap = MainGame::getInstance().GetMap();
	if (!pMap) return;

	const auto& rooms = pMap->GetRooms();
	for (auto* room : rooms) {
		if (room->GetRoomType() == BOSS) {
			int gridSize = FRAME_SIZE * DRAW_SCALE;
			float centerX = (float)(room->GetRx() + room->GetRw() / 2) * gridSize;
			float centerY = (float)(room->GetRy() + room->GetRh() / 2 + 1) * gridSize;

			obj.Position = { centerX, centerY };
			m_vTargetPos = obj.Position;
			m_vStartPos = obj.Position;
			m_isMoving = false;
			moveProgress = 0.0f;
			
			std::cout << "[Debug] Teleported to Boss Room Center: (" << centerX << ", " << centerY << ")" << std::endl;
			break;
		}
	}
}
