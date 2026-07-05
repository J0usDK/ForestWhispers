#pragma once
#include "Systems/UI/Types/InventoryUIRowType.h"

using uint32 = unsigned int;

struct SUIItemData
{
	uint32 nameStringID;
	float weight;
	UITypes::EInventoryUIRowType rowType;
};