#pragma once
#include <vector>
#include "Systems/UI/Types/ItemData.h"

class IInventoryPageViewListener
{
public:
	virtual ~IInventoryPageViewListener() = default;

	virtual void OnInventoryDataUpdated(const std::vector<SUIItemData>& itemsSnapshot) = 0;
};