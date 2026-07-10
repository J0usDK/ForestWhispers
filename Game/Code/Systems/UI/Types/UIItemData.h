#pragma once
#include "Systems/Items/Database/ItemTypes.h"
#include "Systems/UI/Types/InventoryUIRowType.h"

using uint32 = unsigned int;

struct SUIItemData
{
	uint32 nameStringID;
	float weight;
	EItemType itemType;
	UITypes::EInventoryUIRowType rowType;

	SUIItemData() = default;
	SUIItemData(uint32 nameID, float w, EItemType type, UITypes::EInventoryUIRowType row)
		: nameStringID(nameID), weight(w), itemType(type), rowType(row) { }
};