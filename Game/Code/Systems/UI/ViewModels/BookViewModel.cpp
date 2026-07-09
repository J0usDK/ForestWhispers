#include "StdAfx.h"
#include "BookViewModel.h"

void CBookViewModel::SetListener(IBookViewListener* pListener)
{
	m_pListener = pListener;
}

void CBookViewModel::Reset()
{
	m_bBookVisible = false;
}

void CBookViewModel::ToggleBook()
{
	if (!m_pListener)
		return;

	m_bBookVisible = !m_bBookVisible;
	m_pListener->OnBookStateChanged(m_bBookVisible);
}