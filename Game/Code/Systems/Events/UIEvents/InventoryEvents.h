#pragma once
#include <vector>
#include "Systems/UI/Types/ItemData.h"

struct SInventoryUpdateEvent
{
	std::vector<SUIItemData> itemsSnapshot;
};

struct SInventoryItemAddedEvent
{
	SUIItemData itemData;
};

struct SInventoryItemRemovedEvent
{
	int itemIndex;
};