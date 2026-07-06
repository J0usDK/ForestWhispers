#include "StdAfx.h"
#include "InteractionModule.h"

#include "Services/Base/Interaction/InteractionService.h"
#include "Services/Handlers/LootInteractionHandler.h"

CInteractionModule::CInteractionModule() = default;
CInteractionModule::~CInteractionModule() = default;

EBootstrapPhase CInteractionModule::GetPhase() const { return EBootstrapPhase::CoreSystems; }

bool CInteractionModule::Initialize()
{
	m_pService = std::make_unique<CInteractionService>();
	m_pLootHandler = std::make_unique<CLootInteractionHandler>();

	m_pService->RegisterHandler(EInteractionType::Item, m_pLootHandler.get());
	return true;
}

void CInteractionModule::Shutdown()
{
	m_pService->UnregisterHandler(EInteractionType::Item);
	m_pService.reset();
	m_pLootHandler.reset();
}

CInteractionService* CInteractionModule::GetService() { return m_pService.get(); }