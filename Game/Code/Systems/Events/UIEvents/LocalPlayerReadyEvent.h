#pragma once

class IInventoryEventSender;

struct SLocalPlayerReadyEvent
{
	IInventoryEventSender* pInventoryEventSender = nullptr;
};