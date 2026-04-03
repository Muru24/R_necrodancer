#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define ANIM_SPEED          150
#define MOVE_SPEED          10

#define FRAME_SIZE          24
#define DRAW_SCALE          3

#define SCREEN_WIDTH        1280
#define SCREEN_HEIGHT       720

#define MAP_WIDTH           60
#define MAP_HEIGHT          40

#define Tile_DEFAULT_X      48
#define Tile_DEFAULT_Y      0

#define WALL_DEFAULT_X      0
#define WALL_DEFAULT_Y      0
#define WALL_DEFAULT_RANGE  15     
#define WALL_HARD_X         696
#define WALL_HARD_Y         0
#define WALL_BADROCK_X      120
#define WALL_BADROCK_Y      288
#define WALL_SHOP_X         0
#define WALL_SHOP_Y         288

#define SPRITEPATH_FLOORS       L"Sprite/Floor/Floors.png"
#define SPRITEPATH_WALLS        L"Sprite/Floor/Walls.png"

#define SPRITEPATH_CHARACTERS   L"Sprite/Characters.png"
#define SPRITEPATH_SLIMES       L"Sprite/Monster/Monsters_Slimes.png"
#define SPRITEPATH_WRAITHS      L"Sprite/Monster/Monsters_Wraiths.png"
#define SPRITEPATH_SKELETONS    L"Sprite/Monster/Monsters_Skeletons.png"
#define SPRITEPATH_DRAGONS      L"Sprite/Monster/Monsters_Dragons.png"
#define SPRITEPATH_MINOTAURS    L"Sprite/Monster/Monsters_Minotaurs.png"

#define SPRITEPATH_ARMOR        L"Sprite/Item/items_Armor.png"
#define SPRITEPATH_CONSUMABLES  L"Sprite/Item/items_Consumables.png"
#define SPRITEPATH_FOOTWEAR     L"Sprite/Item/items_Footwear.png"
#define SPRITEPATH_HEADWEAR		L"Sprite/Item/items_Headwear.png"
#define SPRITEPATH_RESOURCES	L"Sprite/Item/items_Resources.png"
#define SPRITEPATH_SHOVELS		L"Sprite/Item/items_Shovels.png"
#define SPRITEPATH_TORCHES		L"Sprite/Item/items_Torches.png"
#define SPRITEPATH_WEAPONS		L"Sprite/Item/items_Weapons.png"

enum TileType
{
	TILE_EMPTY = 0,
	TILE_WALL_DEFULT,
	TILE_WALL_HARD,
	TILE_WALL_BADROCK,
	TILE_WALL_SHOP,
	TILE_FLOOR
};
