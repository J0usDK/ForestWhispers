#pragma once
#include "ViewModels/HUDViewModel.h"
#include "ViewModels/TipsViewModel.h"
#include "ViewModels/BookViewModel.h"
#include "ViewModels/InventoryPageViewModel.h"


#include "Views/HUDUIView.h"
#include "Views/TipsUIView.h"
#include "Views/BookUIView.h"
#include "Views/InventoryPageUIView.h"

#include "Systems/Events/UIEvents/ShowHUDEvent.h"
#include "Systems/Events/UIEvents/ShowCrosshairEvent.h"
#include "Systems/Events/UIEvents/ShowHintEvent.h"
#include "Systems/Events/UIEvents/ToggleBookEvent.h"
#include "Systems/Events/UIEvents/InventoryEvents.h"
#include "Systems/Events/UIEvents/LocalPlayerReadyEvent.h"

class CUISystem
{
public:
	CUISystem();
	~CUISystem() = default;

	void HandleEvent(const SShowHUDEvent& event);
	void HandleEvent(const SShowCrosshairEvent& event);
	void HandleEvent(const SShowHintEvent& event);
	void HandleEvent(const SToggleBookEvent& event);
	void HandleEvent(const SInventoryUpdateEvent& event);
	void HandleEvent(const SInventoryItemAddedEvent& event);
	void HandleEvent(const SInventoryItemRemovedEvent& event);
	void HandleEvent(const SLocalPlayerReadyEvent& event);

private:
	std::unique_ptr<CHUDViewModel> m_pHUDVM;
	std::unique_ptr<CTipsViewModel> m_pTipsVM;
	std::unique_ptr<CBookViewModel> m_pBookVM;
	std::unique_ptr<CInventoryPageViewModel> m_pInventoryPageViewModel;

	std::unique_ptr<CHUDUIView> m_pHUDView;
	std::unique_ptr<CTipsUIView> m_pTipsView;
	std::unique_ptr<CBookUIView> m_pBookView;
	std::unique_ptr<CInventoryPageUIView> m_pInventoryPageView;
};