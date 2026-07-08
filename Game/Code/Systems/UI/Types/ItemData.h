#pragma once
#include "Systems/Items/Database/ItemTypes.h"
#include "Systems/UI/Types/InventoryUIRowType.h"

using uint32 = unsigned int;

struct SUIItemData
{
	uint32 nameStringID;
	float weight;
	ItemType itemType;
	UITypes::EInventoryUIRowType rowType;
};