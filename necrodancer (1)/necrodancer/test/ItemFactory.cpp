#include "ItemFactory.h"
#include "Define.h"

ItemBase* ItemFactory::Create(ItemID id, int variantIdx)
{
	ItemBase* pItem = nullptr;
	int srcX = 0;
	int srcY = 0;
	int frameW = 24;
	int frameH = 24;

	switch (id)
	{
	case ITEM_DAGGER:
		pItem = new Weapon(id, L"단검", 1, { {0, 1} }, MAT_BASE);
		srcX = ITEM_WEAPON_DAGGER_SCR_X + (ITEM_WEAPON_DAGGER_FRAME_X * variantIdx);
		srcY = ITEM_WEAPON_DAGGER_SCR_Y;
		frameW = ITEM_WEAPON_DAGGER_FRAME_X;
		frameH = ITEM_WEAPON_DAGGER_FRAME_Y;
		break;
	case ITEM_LONGSWORD:
		pItem = new Weapon(id, L"브로드소드", 2.0f, { {0, 1}, {-1, 1}, {1, 1} }, MAT_BASE);
		srcX = ITEM_WEAPON_LONGSWORD_SCR_X + (ITEM_WEAPON_LONGSWORD_FRAME_X * variantIdx);
		srcY = ITEM_WEAPON_LONGSWORD_SCR_Y;
		frameW = ITEM_WEAPON_LONGSWORD_FRAME_X;
		frameH = ITEM_WEAPON_LONGSWORD_FRAME_Y;
		break;
	case ITEM_RAPIER:
		pItem = new Weapon(id, L"레이피어", 2.0f, { {0, 1}, {0, 2} }, MAT_BASE);
		srcX = ITEM_WEAPON_RAPIER_SCR_X + (ITEM_WEAPON_RAPIER_FRAME_X * variantIdx);
		srcY = ITEM_WEAPON_RAPIER_SCR_Y;
		frameW = ITEM_WEAPON_RAPIER_FRAME_X;
		frameH = ITEM_WEAPON_RAPIER_FRAME_Y;
		break;
	case ITEM_GLASS_SWORD:
		pItem = new Weapon(id, L"유리검", 4.0f, { {0, 1} }, MAT_GLASS);
		srcX = 48 + (24 * 1); 
		srcY = 0;
		frameW = 24;
		frameH = 24;
		break;
	case ITEM_TORCH_BASIC:
		pItem = new Torch(id, L"일반 횃불", 1);
		srcX = 0 + (ITEM_TORCH_FRAME_X * variantIdx);
		srcY = 0;
		frameW = ITEM_TORCH_FRAME_X;
		frameH = ITEM_TORCH_FRAME_Y;
		break;
	case ITEM_TORCH_BRIGHT:
		pItem = new Torch(id, L"밝은 횃불", 2);
		srcX = 24 + (ITEM_TORCH_FRAME_X * variantIdx);
		srcY = 0;
		frameW = ITEM_TORCH_FRAME_X;
		frameH = ITEM_TORCH_FRAME_Y;
		break;
	case ITEM_ARMOR_LEATHER:
		pItem = new Armor(id, L"천갑옷", 0.5f);
		srcX = 0 + (ITEM_ARMOR_FRAME * variantIdx);
		srcY = 0;
		frameW = ITEM_ARMOR_FRAME;
		frameH = ITEM_ARMOR_FRAME;
		break;
	case ITEM_ARMOR_CHAINMAIL:
		pItem = new Armor(id, L"사슬갑옷", 1.0f);
		srcX = 26 + (ITEM_ARMOR_FRAME * variantIdx);
		srcY = 0;
		frameW = ITEM_ARMOR_FRAME;
		frameH = ITEM_ARMOR_FRAME;
		break;
	case ITEM_ARMOR_PLATE:
		pItem = new Armor(id, L"판금갑옷", 1.5f);
		srcX = 52 + (ITEM_ARMOR_FRAME * variantIdx);
		srcY = 0;
		frameW = ITEM_ARMOR_FRAME;
		frameH = ITEM_ARMOR_FRAME;
		break;
	case ITEM_SHOVEL_IRON:
		pItem = new Shovel(id, L"철 삽", 1);
		srcX = 0 + (ITEM_SHOVEIS_FRAME_X * variantIdx);
		srcY = 0;
		frameW = ITEM_SHOVEIS_FRAME_X;
		frameH = ITEM_SHOVEIS_FRAME_Y;
		break;
	case ITEM_SHOVEL_GOLD:
		pItem = new Shovel(id, L"금 삽", 2.0f);
		srcX = 26 + (ITEM_SHOVEIS_FRAME_X * variantIdx);
		srcY = 0;
		frameW = ITEM_SHOVEIS_FRAME_X;
		frameH = ITEM_SHOVEIS_FRAME_Y;
		break;
	case ITEM_SHOVEL_TITANIUM:
		pItem = new Shovel(id, L"티타늄 삽", 3.0f);
		srcX = 52 + (ITEM_SHOVEIS_FRAME_X * variantIdx);
		srcY = 0;
		frameW = ITEM_SHOVEIS_FRAME_X;
		frameH = ITEM_SHOVEIS_FRAME_Y;
		break;
	case ITEM_FOOTWEAR_BASIC:
		pItem = new ItemBase(id, L"기본 신발", SLOT_FEET);
		srcX = 0 + (ITEM_FOOTWEAR_FRAME * variantIdx);
		srcY = 0;
		frameW = ITEM_FOOTWEAR_FRAME;
		frameH = ITEM_FOOTWEAR_FRAME;
		break;
	case ITEM_HEADWEAR_BASIC:
		pItem = new ItemBase(id, L"기본 모자", SLOT_HEAD);
		srcX = 0 + (ITEM_HEADWEAR_FRAME * variantIdx);
		srcY = 0;
		frameW = ITEM_HEADWEAR_FRAME;
		frameH = ITEM_HEADWEAR_FRAME;
		break;
	case ITEM_GOLD_COIN:
		pItem = new ItemBase(id, L"골드", SLOT_ACCESSORY);
		srcX = ITEM_GOLD_SCR_X + (ITEM_GOLD_FRAME * variantIdx);
		srcY = ITEM_GOLD_SCR_Y;
		frameW = ITEM_GOLD_FRAME;
		frameH = ITEM_GOLD_FRAME;
		break;
	case ITEM_BOMB:
		pItem = new ItemBase(id, L"폭탄", SLOT_CONSUMABLE);
		srcX = ITEM_BOMB_SCR_X + (ITEM_BOMB_FRAME * variantIdx);
		srcY = ITEM_BOMB_SCR_Y;
		frameW = ITEM_BOMB_FRAME;
		frameH = ITEM_BOMB_FRAME;
		break;
	case ITEM_CHEESE:
		pItem = new ItemBase(id, L"식량", SLOT_CONSUMABLE);
		srcX = ITEM_CHEESE_SCR_X + (ITEM_CONSUMABLE_FRAME * variantIdx);
		srcY = ITEM_CHEESE_SCR_Y;
		frameW = ITEM_CONSUMABLE_FRAME;
		frameH = ITEM_CONSUMABLE_FRAME;
		break;
	}

	if (pItem) {
		pItem->SetSpriteInfo(srcX, srcY, frameW, frameH);
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
