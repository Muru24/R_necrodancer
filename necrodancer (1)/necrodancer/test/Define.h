#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif


//player
#define ANIM_SPEED          150
#define MOVE_SPEED          10

//Draw_Scale
#define DRAW_SCALE          3
#define UI_BAR_DRAW_SCALE   5
#define NPC_SHOPKEEPER_DRAW_SCALE 4

//ScreenSize
#define SCREEN_WIDTH        1280
#define SCREEN_HEIGHT       800

//UI Pos
#define UI_HP_POS_X			1040
#define UI_HP_POS_Y			10
#define UI_MONEY_POS_X		1120
#define UI_MONEY_POS_Y		10
#define UI_JEWEL_POS_X		1120
#define UI_JEWEL_POS_Y		90
#define UI_INVEN_POS_X		20
#define UI_INVEN_POS_Y		10
#define UI_ITEM_POS_X		20
#define UI_ITEM_POS_Y		110
#define UI_RHYTHM_HEART_POS_X	640
#define UI_RHYTHM_HEART_POS_Y	650
#define UI_RHYTHM_BAR_POS_X 640
#define UI_RHYTHM_BAR_POS_Y 650
#define UI_MONEY_FONT_POS_X	1180
#define UI_MONEY_FONT_POS_Y	48
#define UI_JEWEL_FONT_POS_X	1180
#define UI_JEWEL_FONT_POS_Y	118

//Map Size
#define MAP_WIDTH           60
#define MAP_HEIGHT          40

//Tile Frame
#define TILE_DEFAULT_X      48
#define TILE_DEFAULT_Y      0

//Wall Frame
#define WALL_DEFAULT_X      0
#define WALL_DEFAULT_Y      0
#define WALL_HARD_X         696
#define WALL_HARD_Y         0
#define WALL_BADROCK_X      120
#define WALL_BADROCK_Y      288
#define WALL_SHOP_X         0
#define WALL_SHOP_Y         288
#define WALL_DEFAULT_RANGE  15     

//UI_Sprtie_Pos
#define UI_HP_SCR_X 0
#define UI_HP_SCR_Y 0
#define UI_NULL_HP_SCR_X 58
#define UI_NULL_HP_SCR_Y 0
#define UI_MONEY_SCR_X 85
#define UI_MONEY_SCR_Y 24
#define UI_JEWEL_SCR_X 88
#define UI_JEWEL_SCR_Y 0
#define UI_RHYTHM_HEART_SCR_X 115
#define UI_RHYTHM_HEART_SCR_Y 0
#define UI_RHYTHM_BAR_SCR_X 200
#define UI_RHYTHM_BAR_SCR_Y 9

//UI_Frame
#define FRAME_SIZE          24
#define UI_DRAW_SCALE 1.5f
#define UI_INVEN_FRAME_X 31
#define UI_INVEN_FRAME_Y 40
#define UI_GAP   4.0f
#define UI_INVEN_PNG_X   0
#define UI_INVEN_PNG_Y   51
#define UI_ITEM_GAP_Y 8.0f
#define UI_ITEM_PNG_X 0
#define UI_ITEM_PNG_Y 96
#define UI_ITEM_FRAME_PNG_X 31
#define UI_ITEM_FRAME_PNG_Y 35
#define UI_RHYTHM_HEART_FRAME_X 41
#define UI_RHYTHM_HEART_FRAME_Y 50
#define UI_RHYTHM_BAR_FRAME_X 7
#define UI_RHYTHM_BAR_FRAME_Y 31

//NPC_Frame
#define NPC_SHOPKEEPER_FRAME_X 47
#define NPC_SHOPKEEPER_FRAME_Y 39

//Rhythm
#define RHYTHM_BPM 120
#define RHYTHM_MARGIN	0.2f

//Item_Frame
#define ITEM_ARMOR_FRAME 26
#define ITEM_FOOTWEAR_FRAME 26
#define ITEM_HEADWEAR_FRAME 26
#define ITEM_SHOVEIS_FRAME_X 29
#define ITEM_SHOVEIS_FRAME_Y 26
#define ITEM_TORCH_FRAME_X 23
#define ITEM_TORCH_FRAME_Y 26
#define ITEM_WEAPON_DAGGER_FRAME_X 18
#define ITEM_WEAPON_DAGGER_FRAME_Y 17
#define ITEM_WEAPON_LONGSWORD_FRAME_X 29
#define ITEM_WEAPON_LONGSWORD_FRAME_Y 24
#define ITEM_WEAPON_RAPIER_FRAME_X 29
#define ITEM_WEAPON_RAPIER_FRAME_Y 25
#define ITEM_GOLD_FRAME

//item pos
#define ITEM_WEAPON_DAGGER_SCR_X 0
#define ITEM_WEAPON_DAGGER_SCR_Y 0
#define ITEM_WEAPON_LONGSWORD_SCR_X 0
#define ITEM_WEAPON_LONGSWORD_SCR_Y 54
#define ITEM_WEAPON_RAPIER_SCR_X 0
#define ITEM_WEAPON_RAPIER_SCR_Y 115


#define SPRITEPATH_FLOORS       L"Sprite/Floor/Floors.png"
#define SPRITEPATH_WALLS        L"Sprite/Floor/Walls.png"

#define SPRITEPATH_CHARACTERS   L"Sprite/Characters.png"
#define SPRITEPATH_SLIMES       L"Sprite/Monster/Monsters_Slimes.png"
#define SPRITEPATH_WRAITHS      L"Sprite/Monster/Monsters_Wraiths.png"
#define SPRITEPATH_SKELETONS    L"Sprite/Monster/Monsters_Skeletons.png"
#define SPRITEPATH_DRAGONS      L"Sprite/Monster/Monsters_Dragons.png"
#define SPRITEPATH_MINOTAURS    L"Sprite/Monster/Monsters_Minotaurs.png"
#define SPRITEPATH_SHOPKEPPER   L"Sprite/NPC/NPC_Shopkeeper.png"

#define SPRITEPATH_ARMOR        L"Sprite/Item/items_Armor.png"
#define SPRITEPATH_CONSUMABLES  L"Sprite/Item/items_Consumables.png"
#define SPRITEPATH_FOOTWEAR     L"Sprite/Item/items_Footwear.png"
#define SPRITEPATH_HEADWEAR		L"Sprite/Item/items_Headwear.png"
#define SPRITEPATH_RESOURCES	L"Sprite/Item/items_Resources.png"
#define SPRITEPATH_SHOVELS		L"Sprite/Item/items_Shovels.png"
#define SPRITEPATH_TORCHES		L"Sprite/Item/items_Torches.png"
#define SPRITEPATH_WEAPONS		L"Sprite/Item/items_Weapons.png"

#define SPRITEPATH_HUD			L"Sprite/Ui/HUD.png"



enum TileType
{
	TILE_EMPTY = 0,
	TILE_WALL_DEFAULT,
	TILE_WALL_HARD,
	TILE_WALL_BADROCK,
	TILE_WALL_SHOP,
	TILE_FLOOR
};

enum Visibility {
	VIS_HIDDEN,
	VIS_EXPLORED,
	VIS_VISIBLE
};

enum ItemID {
	ITEM_NONE,
	ITEM_DAGGER,
	ITEM_LONGSWORD,
	ITEM_TORCH_BASIC,
	ITEM_TORCH_BRIGHT,
	ITEM_ARMOR_LEATHER,
	ITEM_SHOVEL_IRON,
	ITEM_SHOVEL_GOLD,
	ITEM_GLASS_SWORD,
	ITEM_RAPIER,
	ITEM_COUNT
};

enum AbilityTrigger {
	TRIGGER_ON_ATTACK,
	TRIGGER_ON_DAMAGED,
	TRIGGER_ON_MOVE,
	TRIGGER_ON_DIG,
	TRIGGER_ON_KILL
};

enum ItemSlot {
	SLOT_WEAPON,
	SLOT_HEAD,
	SLOT_BODY,
	SLOT_FEET,
	SLOT_SHOVEL,
	SLOT_ACCESSORY,
	SLOT_CONSUMABLE,
	SLOT_COUNT
};

enum ItemMaterial {
	MAT_BASE,
	MAT_OBSIDIAN,
	MAT_GLASS,
	MAT_GOLD,
	MAT_BLOOD,
	MAT_TITANIUM
};

