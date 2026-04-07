#pragma once
#include "Item.h"

namespace ItemFactory
{
	ItemBase* Create(ItemID id, int variantIdx = 0);
	const wchar_t* GetAtlasPath(ItemSlot slot);
};
