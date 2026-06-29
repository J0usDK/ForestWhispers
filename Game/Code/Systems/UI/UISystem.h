#pragma once
#include "ViewModels/HUDViewModel.h"
#include "ViewModels/TipsViewModel.h"

#include "Views/HUDUIView.h"
#include "Views/TipsUIView.h"

#include "Systems/Events/UIEvents/ShowHUDEvent.h"
#include "Systems/Events/UIEvents/ShowCrosshairEvent.h"
#include "Systems/Events/UIEvents/ShowHintEvent.h"

class CUISystem
{
public:
	CUISystem();
	~CUISystem() = default;

	void HandleEvent(const SShowHUDEvent& event);
	void HandleEvent(const SShowCrosshairEvent& event);
	void HandleEvent(const SShowHintEvent& event);

private:
	std::unique_ptr<CHUDViewModel> m_pHUDVM;
	std::unique_ptr<CTipsViewModel> m_pTipsVM;

	std::unique_ptr<CHUDUIView> m_pHUDView;
	std::unique_ptr<CTipsUIView> m_pTipsView;
};