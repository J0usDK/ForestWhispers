#include "StdAfx.h"
#include "UISystem.h"
#include "Global/GameEnv.h"
#include "Systems/UI/UIStringTable.h"

CUISystem::CUISystem()
{
	m_pHUDVM = std::make_unique<CHUDViewModel>();
	m_pTipsVM = std::make_unique<CTipsViewModel>();

	m_pHUDView = std::make_unique<CHUDUIView>();
	m_pTipsView = std::make_unique<CTipsUIView>();

	m_pHUDVM->SetListener(m_pHUDView.get());
	m_pTipsVM->SetListener(m_pTipsView.get());
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