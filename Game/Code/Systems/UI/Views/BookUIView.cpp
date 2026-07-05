#include "StdAfx.h"
#include "BookUIView.h"

bool CBookUIView::Initialize()
{
	if (gEnv->pFlashUI)
		m_pUIElement = gEnv->pFlashUI->GetUIElement("BookUI");

	if (!m_pUIElement)
		return false;

	m_pUIElement->SetVisible(true);
	return true;
}

void CBookUIView::OnBookStateChanged(bool isVisible)
{
	if (!m_pUIElement && !Initialize())
		return;

	if (isVisible)
		m_pUIElement->CallFunction("OpenBook");
	else
		m_pUIElement->CallFunction("CloseBook");
}