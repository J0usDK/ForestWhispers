#include "StdAfx.h"
#include "TipsViewModel.h"

void CTipsViewModel::SetListener(ITipsViewListener* pListener)
{
	m_pListener = pListener;
}

void CTipsViewModel::Show(const char* text)
{
	m_prompt = text ? text : "";
	m_isVisible = true;
	NotifyListener();
}

void CTipsViewModel::Hide()
{
	if (!m_isVisible)
		return;

	m_isVisible = false;
	NotifyListener();
}

void CTipsViewModel::NotifyListener()
{
	if (m_pListener)
		m_pListener->OnTipsStateChanged(m_prompt.c_str(), m_isVisible);
}