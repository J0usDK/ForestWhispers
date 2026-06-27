#pragma once

#include "InteractionContext.h"

class IInteractionHandler
{
public:
	virtual ~IInteractionHandler() = default;

	virtual void OnInteract(const SInteractionContext& ctx) = 0;
	virtual bool CanInteract(const SInteractionContext& ctx) = 0;
};