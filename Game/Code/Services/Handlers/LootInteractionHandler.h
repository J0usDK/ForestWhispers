#pragma once

#include "Services/Base/IInteractionHandler.h"

class CLootInteractionHandler : public IInteractionHandler
{
public:
	virtual bool CanInteract(const SInteractionContext& ctx) override;
	virtual void OnInteract(const SInteractionContext& ctx) override;
};