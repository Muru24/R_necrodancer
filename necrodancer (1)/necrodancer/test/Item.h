#pragma once
#include "Define.h"
#include "Struct.h"
#include <string>
#include <vector>

class Player;
class UnitBase;

class ItemBase
{
protected:
	ItemID m_id;
	std::wstring m_name;
	ItemSlot m_slot;
	ItemMaterial m_material;
	float m_baseDamage;
	float m_protection;
	int m_visionBonus;
	float m_digStrength;
	int m_srcX, m_srcY, m_srcW, m_srcH;
	int m_Price;

public:
	ItemBase(ItemID id, std::wstring name, ItemSlot slot, ItemMaterial mat = MAT_BASE);
	virtual ~ItemBase() {}

	virtual void OnEquip(Player* pPlayer) {}
	virtual void OnUnequip(Player* pPlayer) {}
	
	virtual void ApplySpecialAbility(AbilityTrigger trigger, UnitBase* pOwner, UnitBase* pTarget);

	ItemID GetID() const { return m_id; }
	const std::wstring& GetName() const { return m_name; }
	ItemSlot GetSlot() const { return m_slot; }
	ItemMaterial GetMaterial() const { return m_material; }

	float GetBaseDamage() const { return m_baseDamage; }
	float GetProtection() const { return m_protection; }
	int GetVisionBonus() const { return m_visionBonus; }
	float GetDigStrength() const { return m_digStrength; }

	void SetSpriteInfo(int x, int y, int w, int h) { m_srcX = x; m_srcY = y; m_srcW = w; m_srcH = h; }
	int GetSrcX() const { return m_srcX; }
	int GetSrcY() const { return m_srcY; }
	int GetSrcW() const { return m_srcW; }
	int GetSrcH() const { return m_srcH; }

	void SetPrice(int price) { m_Price = price; }
	int GetPrice() const { return m_Price; }

	void GetItem(Player* player);
};

class Weapon : public ItemBase
{
private:
	std::vector<Vector2> m_attackRange;

public:
	Weapon(ItemID id, std::wstring name, float damage, std::vector<Vector2> range, ItemMaterial mat = MAT_BASE);

	const std::vector<Vector2>& GetAttackRange() const { return m_attackRange; }
};

class Torch : public ItemBase
{
public:
	Torch(ItemID id, std::wstring name, int visionBonus);
	virtual void OnEquip(Player* pPlayer) override;
	virtual void OnUnequip(Player* pPlayer) override;
};

class Armor : public ItemBase
{
public:
	Armor(ItemID id, std::wstring name, float protection);
};

class Shovel : public ItemBase
{
public:
	Shovel(ItemID id, std::wstring name, float digStrength);
};
