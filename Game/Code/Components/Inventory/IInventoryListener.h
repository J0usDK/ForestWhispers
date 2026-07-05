#pragma once
#include <vector>
#include "Systems/UI/Types/ItemData.h"

class IInventoryListener
{
public:
	virtual ~IInventoryListener() = default;

	virtual void OnInventoryUpdated(const std::vector<SUIItemData>& itemsSnapshot) = 0;
	virtual void OnInventoryItemAdded(const SUIItemData& newItem) = 0;
	virtual void OnInventoryItemRemoved(int itemIndex) = 0;
};