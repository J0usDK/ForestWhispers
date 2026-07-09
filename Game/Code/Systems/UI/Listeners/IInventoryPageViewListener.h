#pragma once
#include <vector>

struct SUIItemData;

class IInventoryPageViewListener
{
public:
	virtual ~IInventoryPageViewListener() = default;

	virtual void OnInventoryDataUpdated(const std::vector<SUIItemData>& itemsSnapshot) = 0;
};