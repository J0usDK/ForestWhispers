#include "StdAfx.h"
#include "UISystem.h"
#include "Global/GameEnv.h"
#include "Systems/UI/UIStringTable.h"

CUISystem::CUISystem()
{
	m_pTipsVM = std::make_unique<CTipsViewModel>();
	m_pTipsView = std::make_unique<CTipsUIView>();

	m_pTipsVM->SetListener(m_pTipsView.get());
}

void CUISystem::HandleEvent(const SShowHintEvent& event)
{
	string text = gGameEnv->pUIStringTable->Resolve(event.stringKey);
	if (event.isVisible)
		m_pTipsVM->Show(text.c_str());
	else
		m_pTipsVM->Hide();
}