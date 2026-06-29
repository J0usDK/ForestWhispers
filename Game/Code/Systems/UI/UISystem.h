#pragma once
#include "ViewModels/TipsViewModel.h"
#include "Views/TipsUIView.h"
#include "Systems/Events/UIEvents/ShowHintEvent.h"

class CUISystem
{
public:
	CUISystem();
	~CUISystem() = default;

	void HandleEvent(const SShowHintEvent& event);

private:
	std::unique_ptr<CTipsViewModel> m_pTipsVM;

	std::unique_ptr<CTipsUIView> m_pTipsView;
};