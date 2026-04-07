#include "Item.h"
#include "Player.h"

void ItemBase::ApplySpecialAbility(AbilityTrigger trigger, UnitBase* pOwner, UnitBase* pTarget)
{
	switch (m_material)
	{
	case MAT_GLASS:
		if (trigger == TRIGGER_ON_ATTACK) {
			if (pOwner->GetTag() == PLAYER) {
				static_cast<Player*>(pOwner)->DestroyItem(this);
			}
		}
		break;
	case MAT_GOLD:
		if (trigger == TRIGGER_ON_KILL) {
		}
		break;
	case MAT_BLOOD:
		if (trigger == TRIGGER_ON_KILL) {
		}
		break;
	}
}

void ItemBase::GetItem(Player* player)
{
	if (m_Price > player->GetMoney()) return;

	player->SetMoney(player->GetMoney() - m_Price);
	OnUnequip(player);
	OnEquip(player);
}


Weapon::Weapon(ItemID id, std::wstring name, float damage, std::vector<Vector2> range, ItemMaterial mat)
	: ItemBase(id, name, SLOT_WEAPON, mat), m_attackRange(range)
{
	m_baseDamage = damage;
}

Torch::Torch(ItemID id, std::wstring name, int visionBonus)
	: ItemBase(id, name, SLOT_TORCH)
{
	m_visionBonus = visionBonus;
}

void Torch::OnEquip(Player* pPlayer)
{
	ItemBase::OnEquip(pPlayer);
	if (pPlayer) {
		pPlayer->SetVisionRadius(pPlayer->GetVisionRadius() + m_visionBonus);
	}
}

void Torch::OnUnequip(Player* pPlayer)
{
	ItemBase::OnUnequip(pPlayer);
	if (pPlayer) {
		pPlayer->SetVisionRadius(pPlayer->GetVisionRadius() - m_visionBonus);
	}
}

Armor::Armor(ItemID id, std::wstring name, float protection)
	: ItemBase(id, name, SLOT_BODY)
{
	m_protection = protection;
}

Shovel::Shovel(ItemID id, std::wstring name, float digStrength)
	: ItemBase(id, name, SLOT_SHOVEL)
{
	m_digStrength = digStrength;
}
