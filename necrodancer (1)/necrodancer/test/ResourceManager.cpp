#include "ResourceManager.h"

using namespace Gdiplus;

ResourceManager::ResourceManager()
	: m_pTileImg(nullptr), m_pSpriteAtlas(nullptr), m_pWallImg(nullptr), m_pSlimeImg(nullptr),
	m_pSkeletonImg(nullptr), m_pBatImg(nullptr), m_HUD(nullptr), m_pNote(nullptr),
	m_pShopkeeper(nullptr), m_pEffectAttack(nullptr), m_pItemWeapons(nullptr),
	m_pItemArmor(nullptr), m_pItemHeadwear(nullptr), m_pItemFootwear(nullptr),
	m_pItemShovels(nullptr), m_pItemTorches(nullptr), m_pItemResources(nullptr),
	m_pItemConsumables(nullptr), m_pTitleImg(nullptr), m_pDeadRingerImg(nullptr), m_pDefaultFont(nullptr),
	m_pBigFont(nullptr), m_pSelectedFont(nullptr), m_pWhiteBrush(nullptr), m_pBlackBrush(nullptr)
{
}

ResourceManager::~ResourceManager()
{
	Release();
}

void ResourceManager::Init()
{
	m_pSpriteAtlas = new Gdiplus::Image(SPRITEPATH_CHARACTERS);
	m_pTileImg = new Gdiplus::Image(SPRITEPATH_FLOORS);
	m_pWallImg = new Gdiplus::Image(SPRITEPATH_WALLS);
	m_pSlimeImg = new Gdiplus::Image(SPRITEPATH_SLIMES);
	m_pSkeletonImg = new Gdiplus::Image(SPRITEPATH_SKELETONS);
	m_pBatImg = new Gdiplus::Image(SPRITEPATH_BAT);
	m_HUD = new Gdiplus::Image(SPRITEPATH_HUD);
	m_pShopkeeper = new Gdiplus::Image(SPRITEPATH_SHOPKEPPER);

	m_pDefaultFont = new Gdiplus::Font(L"맑은 고딕", 12, FontStyleBold);
	m_pBigFont = new Gdiplus::Font(L"맑은 고딕", 24, FontStyleBold);
	m_pSelectedFont = new Gdiplus::Font(L"맑은 고딕", 32, FontStyleBold);
	m_pWhiteBrush = new Gdiplus::SolidBrush(Color(255, 255, 255, 255));
	m_pBlackBrush = new Gdiplus::SolidBrush(Color(255, 0, 0, 0));

	m_pItemWeapons = new Gdiplus::Image(SPRITEPATH_WEAPONS);
	m_pItemArmor = new Gdiplus::Image(SPRITEPATH_ARMOR);
	m_pItemHeadwear = new Gdiplus::Image(SPRITEPATH_HEADWEAR);
	m_pItemFootwear = new Gdiplus::Image(SPRITEPATH_FOOTWEAR);
	m_pItemShovels = new Gdiplus::Image(SPRITEPATH_SHOVELS);
	m_pItemTorches = new Gdiplus::Image(SPRITEPATH_TORCHES);
	m_pItemResources = new Gdiplus::Image(SPRITEPATH_RESOURCES);
	m_pItemConsumables = new Gdiplus::Image(SPRITEPATH_CONSUMABLES);
	m_pTitleImg = new Gdiplus::Image(SPRITEPATH_SCENE_TITLE);
	m_pEffectAttack = new Gdiplus::Image(SPRITEPATH_EFFECT_ATTACK);
	m_pDeadRingerImg = new Gdiplus::Image(SPRITEPATH_Dead_Ringer);
}

void ResourceManager::Release()
{
	if (m_pSpriteAtlas)    { delete m_pSpriteAtlas; m_pSpriteAtlas = nullptr; }
	if (m_pTileImg)        { delete m_pTileImg; m_pTileImg = nullptr; }
	if (m_pWallImg)        { delete m_pWallImg; m_pWallImg = nullptr; }
	if (m_pSlimeImg)       { delete m_pSlimeImg; m_pSlimeImg = nullptr; }
	if (m_pSkeletonImg)    { delete m_pSkeletonImg; m_pSkeletonImg = nullptr; }
	if (m_pBatImg)         { delete m_pBatImg; m_pBatImg = nullptr; }
	if (m_HUD)             { delete m_HUD; m_HUD = nullptr; }
	if (m_pShopkeeper)     { delete m_pShopkeeper; m_pShopkeeper = nullptr; }
	if (m_pNote)           { delete m_pNote; m_pNote = nullptr; }
	if (m_pEffectAttack)   { delete m_pEffectAttack; m_pEffectAttack = nullptr; }

	if (m_pDefaultFont)    { delete m_pDefaultFont; m_pDefaultFont = nullptr; }
	if (m_pBigFont)        { delete m_pBigFont; m_pBigFont = nullptr; }
	if (m_pSelectedFont)   { delete m_pSelectedFont; m_pSelectedFont = nullptr; }
	if (m_pWhiteBrush)     { delete m_pWhiteBrush; m_pWhiteBrush = nullptr; }
	if (m_pBlackBrush)     { delete m_pBlackBrush; m_pBlackBrush = nullptr; }

	if (m_pItemWeapons)    { delete m_pItemWeapons; m_pItemWeapons = nullptr; }
	if (m_pItemArmor)      { delete m_pItemArmor; m_pItemArmor = nullptr; }
	if (m_pItemHeadwear)   { delete m_pItemHeadwear; m_pItemHeadwear = nullptr; }
	if (m_pItemFootwear)   { delete m_pItemFootwear; m_pItemFootwear = nullptr; }
	if (m_pItemShovels)    { delete m_pItemShovels; m_pItemShovels = nullptr; }
	if (m_pItemTorches)    { delete m_pItemTorches; m_pItemTorches = nullptr; }
	if (m_pItemResources)  { delete m_pItemResources; m_pItemResources = nullptr; }
	if (m_pItemConsumables){ delete m_pItemConsumables; m_pItemConsumables = nullptr; }
	if (m_pTitleImg)       { delete m_pTitleImg; m_pTitleImg = nullptr; }
	if (m_pDeadRingerImg)  { delete m_pDeadRingerImg; m_pDeadRingerImg = nullptr; }
}
