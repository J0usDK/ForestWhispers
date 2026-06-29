#include "StdAfx.h"
#include "HUDUIView.h"

bool CHUDUIView::Initialize()
{
	if (gEnv->pFlashUI)
		m_pUIElement = gEnv->pFlashUI->GetUIElement("BasicHUD");

	if (!m_pUIElement)
		return false;

	m_pUIElement->SetVisible(true);
	return false;
}

void CHUDUIView::OnCrosshairStateChanged(bool isVisible)
{
	if (!m_pUIElement && !Initialize())
		return;

	if (isVisible)
		m_pUIElement->CallFunction("ShowCrosshair");
	else
		m_pUIElement->CallFunction("HideCrosshair");
}

void CHUDUIView::OnHUDStateChanged(bool isVisible)
{
	if (!m_pUIElement && !Initialize())
		return;

	if (isVisible)
		m_pUIElement->CallFunction("ShowHUD");
	else
		m_pUIElement->CallFunction("HideHUD");
}