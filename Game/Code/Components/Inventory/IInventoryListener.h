#pragma once
#include <vector>

struct SUIItemData;

class IInventoryListener
{
public:
	virtual ~IInventoryListener() = default;

	virtual void OnInventoryUpdated(const std::vector<SUIItemData>& itemsSnapshot) = 0;
	virtual void OnInventoryItemAdded(const SUIItemData& newItem) = 0;
	virtual void OnInventoryItemRemoved(int itemIndex) = 0;
};