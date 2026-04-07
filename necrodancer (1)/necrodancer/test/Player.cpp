#include "Player.h"
#include "Define.h"
#include <windows.h>
#include "Map.h"
#include "MainGame.h"
#include <cmath>
#include "Item.h"
#include <iostream>
#include "RhythmManager.h"

void Player::Move()
{
	if (!RhythmManager::getInstance().IsBeatWindow()) {
		m_bActedThisBeat = false;

		bool currentKeyState[4];
		currentKeyState[0] = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
		currentKeyState[1] = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
		currentKeyState[2] = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
		currentKeyState[3] = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
		for (int i = 0; i < 4; ++i) m_prevKeyState[i] = currentKeyState[i];
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
			if (TryMove(dx, dy)) {
				// 이동 후 아이템 습득 체크
				int px = static_cast<int>(m_vTargetPos.X / gridSize);
				int py = static_cast<int>(m_vTargetPos.Y / gridSize);
				ItemBase* pFloorItem = pMap->PickupItem(px, py);
				if (pFloorItem) {
					Equip(pFloorItem);
				}
			}
			m_bActedThisBeat = true;
		}
	}

	for (int i = 0; i < 4; ++i) {
		m_prevKeyState[i] = currentKeyState[i];
	}
}

void Player::Attack(UnitBase& Target)
{
	float finalDmg = status.Attack;
	ItemBase* pWeapon = GetEquippedItem(SLOT_WEAPON);
	if (pWeapon) {
		finalDmg += pWeapon->GetBaseDamage(); // 기본 공격력 + 무기 공격력
		pWeapon->ApplySpecialAbility(TRIGGER_ON_ATTACK, this, &Target);
	}
	
	Target.TakeDamage(finalDmg);
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

	Map* pMap = MainGame::getInstance().GetMap();
	int gridSize = FRAME_SIZE * DRAW_SCALE;
	int px = static_cast<int>(GetLogicalPos().X / gridSize);
	int py = static_cast<int>(GetLogicalPos().Y / gridSize);

	for (auto it = m_equips.begin(); it != m_equips.end(); ++it) {
		if (it->first == pItem->GetSlot()) {
			ItemBase* oldItem = it->second;
			oldItem->OnUnequip(this);
			
			// 기존 아이템을 현재 위치에 드랍
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
			// 장착 목록에서 제거만 하고 메모리는 외부(또는 여기서) 해제 결정 필요
			// 여기서는 장착 해제 후 삭제 처리
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

float Player::GetProtection() const
{
	float totalProt = 0.0f;
	ItemBase* pArmor = GetEquippedItem(SLOT_BODY);
	if (pArmor) totalProt += pArmor->GetProtection();
	return totalProt;
}

float Player::GetDigLevel() const
{
	float baseDig = 1.0f; // 기본 맨손(?) 굴착 등급
	ItemBase* pShovel = GetEquippedItem(SLOT_SHOVEL);
	if (pShovel) return pShovel->GetDigStrength();
	return baseDig;
}
