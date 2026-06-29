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

void CInteractionService::ExecuteInteraction(IEntity* actor, const SInteractionFocus& focus)
{
	if (!actor || !focus.IsValid())
		return;

	auto it = m_handlers.find(focus.type);
	if (it == m_handlers.end() || !it->second)
		return;

	SInteractionContext ctx;
	ctx.actor = actor;
	ctx.target = gEnv->pEntitySystem->GetEntity(focus.entityID);

	IInteractionHandler* pHandler = it->second;
	if (pHandler->CanInteract(ctx))
		pHandler->OnInteract(ctx);
}