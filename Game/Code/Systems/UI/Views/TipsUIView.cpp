#include "StdAfx.h"
#include "TipsUIView.h"

CTipsUIView::CTipsUIView()
{
}

bool CTipsUIView::Initialize()
{
	if (gEnv->pFlashUI)
		m_pUIElement = gEnv->pFlashUI->GetUIElement("worldTooltip");

	if (!m_pUIElement)
		return false;

	m_pUIElement->SetVisible(true);
	return true;
}

void CTipsUIView::OnTipsStateChanged(const char* prompt, bool isVisible)
{
	if (!m_pUIElement && !Initialize())
		return;

	if (isVisible && prompt)
	{
		SUIArguments args;
		args.AddArgument(prompt);
		m_pUIElement->CallFunction("Show", args);
	}
	else
		m_pUIElement->CallFunction("Hide");
}