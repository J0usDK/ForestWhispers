#pragma once
#include "IInventoryListener.h"

class IInventoryEventSender
{
public:
	virtual ~IInventoryEventSender() = default;

	virtual void RegisterListener(IInventoryListener* pListener) = 0;
	virtual void UnregisterListener(IInventoryListener* pListener) = 0;
};