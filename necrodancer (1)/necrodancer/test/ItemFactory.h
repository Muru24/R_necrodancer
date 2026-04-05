#pragma once
#include "Item.h"

namespace ItemFactory
{
	ItemBase* Create(ItemID id);
	const wchar_t* GetAtlasPath(ItemSlot slot);
};
