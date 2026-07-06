#include "StdAfx.h"
#include "TipsUIView.h"

CTipsUIView::CTipsUIView()
{
	m_pUIElement = gEnv->pFlashUI->GetUIElement("worldTooltip");
	m_pUIElement->SetVisible(true);
}

void CTipsUIView::OnTipsStateChanged(const char* prompt, bool isVisible)
{
	if (isVisible && prompt)
	{
		SUIArguments args;
		args.AddArgument(prompt);
		m_pUIElement->CallFunction("Show", args);
	}
	else
		m_pUIElement->CallFunction("Hide");
}