#include "StdAfx.h"
#include "HUDUIView.h"

CHUDUIView::CHUDUIView()
{
	m_pUIElement = gEnv->pFlashUI->GetUIElement("BasicHUD");
	m_pUIElement->SetVisible(true);
}

void CHUDUIView::Reset()
{
	m_pUIElement->CallFunction("HideHUD");
}

void CHUDUIView::OnCrosshairStateChanged(bool isVisible)
{
	if (isVisible)
		m_pUIElement->CallFunction("ShowCrosshair");
	else
		m_pUIElement->CallFunction("HideCrosshair");
}

void CHUDUIView::OnHUDStateChanged(bool isVisible)
{
	if (isVisible)
		m_pUIElement->CallFunction("ShowHUD");
	else
		m_pUIElement->CallFunction("HideHUD");
}