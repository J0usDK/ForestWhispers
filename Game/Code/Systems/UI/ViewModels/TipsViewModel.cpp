#include "StdAfx.h"
#include "TipsViewModel.h"

void CTipsViewModel::SetListener(ITipsViewListener* pListener)
{
	m_pListener = pListener;
}

void CTipsViewModel::Reset()
{
	m_prompt = 0;
	m_bIsVisible = false;
}

void CTipsViewModel::Show(uint32 stringID)
{
	m_prompt = stringID;
	m_bIsVisible = true;
	NotifyListener();
}

void CTipsViewModel::Hide()
{
	if (!m_bIsVisible)
		return;

	m_bIsVisible = false;
	NotifyListener();
}

void CTipsViewModel::NotifyListener()
{
	if (m_pListener)
		m_pListener->OnTipsStateChanged(m_prompt, m_bIsVisible);
}