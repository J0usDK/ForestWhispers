#include "StdAfx.h"
#include "BookUIView.h"

CBookUIView::CBookUIView()
{
	m_pUIElement = gEnv->pFlashUI->GetUIElement("BookUI");
	m_pUIElement->SetVisible(true);
}

void CBookUIView::OnBookStateChanged(bool isVisible)
{
	if (isVisible)
		m_pUIElement->CallFunction("OpenBook");
	else
		m_pUIElement->CallFunction("CloseBook");
}