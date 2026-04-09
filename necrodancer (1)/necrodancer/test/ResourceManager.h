#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <string>
#include "Define.h"

class ResourceManager
{
private:
	ResourceManager();
	~ResourceManager();

	// Images
	Gdiplus::Image* m_pTileImg;
	Gdiplus::Image* m_pSpriteAtlas;
	Gdiplus::Image* m_pWallImg;
	Gdiplus::Image* m_pSlimeImg;
	Gdiplus::Image* m_pSkeletonImg;
	Gdiplus::Image* m_pBatImg;
	Gdiplus::Image* m_HUD;
	Gdiplus::Image* m_pNote;
	Gdiplus::Image* m_pShopkeeper;
	Gdiplus::Image* m_pEffectAttack;
	Gdiplus::Image* m_pDeadRingerImg;

	Gdiplus::Image* m_pItemWeapons;
	Gdiplus::Image* m_pItemArmor;
	Gdiplus::Image* m_pItemHeadwear;
	Gdiplus::Image* m_pItemFootwear;
	Gdiplus::Image* m_pItemShovels;
	Gdiplus::Image* m_pItemTorches;
	Gdiplus::Image* m_pItemResources;
	Gdiplus::Image* m_pItemConsumables;
	Gdiplus::Image* m_pTitleImg;

	// Fonts
	Gdiplus::Font* m_pDefaultFont;
	Gdiplus::Font* m_pBigFont;
	Gdiplus::Font* m_pSelectedFont;

	// Brushes
	Gdiplus::SolidBrush* m_pWhiteBrush;
	Gdiplus::SolidBrush* m_pBlackBrush;

public:
	static ResourceManager& getInstance()
	{
		static ResourceManager instance;
		return instance;
	}

	void Init();
	void Release();

	// Image Getters
	Gdiplus::Image* GetTileImg() const { return m_pTileImg; }
	Gdiplus::Image* GetSpriteAtlas() const { return m_pSpriteAtlas; }
	Gdiplus::Image* GetWallImg() const { return m_pWallImg; }
	Gdiplus::Image* GetSlimeImg() const { return m_pSlimeImg; }
	Gdiplus::Image* GetSkeletonImg() const { return m_pSkeletonImg; }
	Gdiplus::Image* GetBatImg() const { return m_pBatImg; }
	Gdiplus::Image* GetHUD() const { return m_HUD; }
	Gdiplus::Image* GetNote() const { return m_pNote; }
	Gdiplus::Image* GetShopkeeper() const { return m_pShopkeeper; }
	Gdiplus::Image* GetEffectAttack() const { return m_pEffectAttack; }
	Gdiplus::Image* GetDeadRingerImg() const { return m_pDeadRingerImg; }

	Gdiplus::Image* GetItemWeapons() const { return m_pItemWeapons; }
	Gdiplus::Image* GetItemArmor() const { return m_pItemArmor; }
	Gdiplus::Image* GetItemHeadwear() const { return m_pItemHeadwear; }
	Gdiplus::Image* GetItemFootwear() const { return m_pItemFootwear; }
	Gdiplus::Image* GetItemShovels() const { return m_pItemShovels; }
	Gdiplus::Image* GetItemTorches() const { return m_pItemTorches; }
	Gdiplus::Image* GetItemResources() const { return m_pItemResources; }
	Gdiplus::Image* GetItemConsumables() const { return m_pItemConsumables; }
	Gdiplus::Image* GetTitleImg() const { return m_pTitleImg; }

	// Font Getters
	Gdiplus::Font* GetDefaultFont() const { return m_pDefaultFont; }
	Gdiplus::Font* GetBigFont() const { return m_pBigFont; }
	Gdiplus::Font* GetSelectedFont() const { return m_pSelectedFont; }

	// Brush Getters
	Gdiplus::SolidBrush* GetWhiteBrush() const { return m_pWhiteBrush; }
	Gdiplus::SolidBrush* GetBlackBrush() const { return m_pBlackBrush; }
};
