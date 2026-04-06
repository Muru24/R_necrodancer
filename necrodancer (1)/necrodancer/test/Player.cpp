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
			TryMove(dx, dy);
			m_bActedThisBeat = true;
		}
	}

	for (int i = 0; i < 4; ++i) {
		m_prevKeyState[i] = currentKeyState[i];
	}
}

void Player::Attack(UnitBase& Target)
{
	int finalDmg = status.Attack;
	ItemBase* pWeapon = GetEquippedItem(SLOT_WEAPON);
	if (pWeapon) {
		finalDmg = pWeapon->GetBaseDamage();
		pWeapon->ApplySpecialAbility(TRIGGER_ON_ATTACK, this, &Target);
	}
	
	Target.TakeDamage(finalDmg);
}

void Player::TakeDamage(int atk)
{
	int finalDmg = atk;
	ItemBase* pArmor = GetEquippedItem(SLOT_BODY);
	if (pArmor) {
		finalDmg -= pArmor->GetProtection();
		if (finalDmg < 0) finalDmg = 0;
		pArmor->ApplySpecialAbility(TRIGGER_ON_DAMAGED, this, nullptr);
	}

	UnitBase::TakeDamage(finalDmg);
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

	for (auto it = m_equips.begin(); it != m_equips.end(); ++it) {
		if (it->first == pItem->GetSlot()) {
			it->second->OnUnequip(this);
			m_equips.erase(it);
			break;
		}
	}

	m_equips.push_back(std::make_pair(pItem->GetSlot(), pItem));
	pItem->OnEquip(this);
}

ItemBase* Player::GetEquippedItem(ItemSlot slot) const
{
	for (const auto& pair : m_equips) {
		if (pair.first == slot) return pair.second;
	}
	return nullptr;
}

int Player::GetDigLevel() const
{
	ItemBase* pShovel = GetEquippedItem(SLOT_SHOVEL);
	if (pShovel) return pShovel->GetDigStrength();
	return 0;
}
