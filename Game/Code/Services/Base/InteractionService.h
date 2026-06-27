#pragma once

#include "IInteractionHandler.h"
#include "Components/Interaction/InteractionComponent.h"

class CInteractionService
{
public:
	CInteractionService() = default;
	~CInteractionService() = default;

	void RegisterHandler(EInteractionType type, IInteractionHandler* pHandler);
	void UnregisterHandler(EInteractionType type);

	void ExecuteInteraction(const SInteractionContext& ctx, EInteractionType type);

private:
	std::unordered_map<EInteractionType, IInteractionHandler*> m_handlers;
};