#pragma once
#include "IInventoryListener.h"

class IInventoryDataProvider
{
public:
	virtual ~IInventoryDataProvider() = default;

	virtual void RegisterListener(IInventoryListener* pListener) = 0;
	virtual void UnregisterListener(IInventoryListener* pListener) = 0;
};