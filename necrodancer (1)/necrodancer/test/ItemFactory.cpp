#include "ItemFactory.h"
#include "Define.h"

ItemBase* ItemFactory::Create(ItemID id)
{
	ItemBase* pItem = nullptr;
	switch (id)
	{
	case ITEM_DAGGER:
		pItem = new Weapon(id, L"단검", 1, { {0, 1} }, MAT_BASE);
		pItem->SetSpriteInfo(0, 0, 24, 24);
		break;
	case ITEM_LONGSWORD:
		pItem = new Weapon(id, L"롱소드", 3, { {0, 1}, {0, 2} }, MAT_BASE);
		pItem->SetSpriteInfo(24, 0, 24, 24);
		break;
	case ITEM_GLASS_SWORD:
		pItem = new Weapon(id, L"유리검", 5, { {0, 1} }, MAT_GLASS);
		pItem->SetSpriteInfo(48, 0, 24, 24);
		break;
	case ITEM_TORCH_BASIC:
		pItem = new Torch(id, L"일반 횃불", 1);
		pItem->SetSpriteInfo(0, 0, 24, 24);
		break;
	case ITEM_TORCH_BRIGHT:
		pItem = new Torch(id, L"밝은 횃불", 2);
		pItem->SetSpriteInfo(24, 0, 24, 24);
		break;
	case ITEM_ARMOR_LEATHER:
		pItem = new Armor(id, L"가죽 갑옷", 1);
		pItem->SetSpriteInfo(0, 0, 24, 24);
		break;
	case ITEM_SHOVEL_IRON:
		pItem = new Shovel(id, L"철 삽", 1);
		pItem->SetSpriteInfo(0, 0, 24, 24);
		break;
	}
	return pItem;
}

const wchar_t* ItemFactory::GetAtlasPath(ItemSlot slot)
{
	switch (slot)
	{
	case SLOT_WEAPON:     return SPRITEPATH_WEAPONS;
	case SLOT_HEAD:       return SPRITEPATH_TORCHES;
	case SLOT_BODY:       return SPRITEPATH_ARMOR;
	case SLOT_FEET:       return SPRITEPATH_FOOTWEAR;
	case SLOT_SHOVEL:     return SPRITEPATH_SHOVELS;
	case SLOT_CONSUMABLE: return SPRITEPATH_CONSUMABLES;
	}
	return nullptr;
}
