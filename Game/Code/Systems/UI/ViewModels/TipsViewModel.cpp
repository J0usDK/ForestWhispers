#include "StdAfx.h"
#include "TipsViewModel.h"

void CTipsViewModel::SetListener(ITipsViewListener* pListener)
{
	m_pListener = pListener;
}

void CTipsViewModel::Show(uint32 stringID)
{
	m_prompt = stringID;
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
		m_pListener->OnTipsStateChanged(m_prompt, m_isVisible);
}