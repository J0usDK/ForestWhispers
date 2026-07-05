#include "StdAfx.h"
#include "UISystem.h"
#include "Global/GameEnv.h"
#include <CryGame/IGameFramework.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <CryEntitySystem/IEntity.h>
#include "Systems/UI/UIStringTable.h"
#include "Components/Inventory/IInventoryEventSender.h"

CUISystem::CUISystem()
{
	m_pHUDVM = std::make_unique<CHUDViewModel>();
	m_pBookVM = std::make_unique<CBookViewModel>();
	m_pTipsVM = std::make_unique<CTipsViewModel>();
	m_pInventoryPageViewModel = std::make_unique<CInventoryPageViewModel>();

	m_pHUDView = std::make_unique<CHUDUIView>();
	m_pBookView = std::make_unique<CBookUIView>();
	m_pTipsView = std::make_unique<CTipsUIView>();
	m_pInventoryPageView = std::make_unique<CInventoryPageUIView>();

	m_pHUDVM->SetListener(m_pHUDView.get());
	m_pBookVM->SetListener(m_pBookView.get());
	m_pTipsVM->SetListener(m_pTipsView.get());
	m_pInventoryPageViewModel->SetListener(m_pInventoryPageView.get());
}

void CUISystem::HandleEvent(const SShowHUDEvent& event)
{
	if (event.isVisible)
		m_pHUDVM->ShowHUD();
	else
		m_pHUDVM->HideHUD();
}

void CUISystem::HandleEvent(const SShowCrosshairEvent& event)
{
	if (event.isVisible)
		m_pHUDVM->ShowCrosshair();
	else
		m_pHUDVM->HideCrosshair();
}

void CUISystem::HandleEvent(const SShowHintEvent& event)
{
	string text = gGameEnv->pUIStringTable->Resolve(event.stringKey);
	if (event.isVisible)
		m_pTipsVM->Show(text.c_str());
	else
		m_pTipsVM->Hide();
}

void CUISystem::HandleEvent(const SToggleBookEvent& event)
{
	m_pBookVM->ToggleBook();
}

void CUISystem::HandleEvent(const SInventoryUpdateEvent& event)
{
	m_pInventoryPageViewModel->OnInventoryUpdated(event.itemsSnapshot);
}

void CUISystem::HandleEvent(const SInventoryItemAddedEvent& event)
{
	m_pInventoryPageViewModel->OnInventoryItemAdded(event.itemData);
}

void CUISystem::HandleEvent(const SInventoryItemRemovedEvent& event)
{
	// For simplicity, we can just request a full inventory update when an item is removed.
	// In a real implementation, you might want to handle this more efficiently.
	// Here we will just log the removal for demonstration purposes.
	CryLog("Inventory item removed at index: %d", event.itemIndex);
}

void CUISystem::HandleEvent(const SLocalPlayerReadyEvent& event)
{
	event.pInventoryEventSender->RegisterListener(m_pInventoryPageViewModel.get());
}