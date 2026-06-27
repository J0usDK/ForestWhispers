#include "StdAfx.h"
#include "InteractionService.h"

void CInteractionService::RegisterHandler(EInteractionType type, IInteractionHandler* pHandler)
{
	if (pHandler)
		m_handlers[type] = pHandler;
}

void CInteractionService::UnregisterHandler(EInteractionType type)
{
	m_handlers.erase(type);
}

void CInteractionService::ExecuteInteraction(const SInteractionContext& ctx, EInteractionType type)
{
	if (!ctx.IsValid())
		return;

	auto it = m_handlers.find(type);
	if (it == m_handlers.end() || !it->second)
		return;

	IInteractionHandler* pHandler = it->second;
	if (pHandler->CanInteract(ctx))
		pHandler->OnInteract(ctx);
}