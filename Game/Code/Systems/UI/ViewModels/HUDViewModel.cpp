#include "StdAfx.h"
#include "HUDViewModel.h"

void CHUDViewModel::SetListener(IHUDViewListener* pListener)
{
	m_pListener = pListener;
}

void CHUDViewModel::Reset()
{
	m_bCrosshairVisible = false;
	m_bHUDVisible = false;
}

void CHUDViewModel::ShowCrosshair()
{
	if (m_bCrosshairVisible)
		return;

	m_bCrosshairVisible = true;
	m_pListener->OnCrosshairStateChanged(m_bCrosshairVisible);
}

void CHUDViewModel::HideCrosshair()
{
	if (!m_bCrosshairVisible)
		return;

	m_bCrosshairVisible = false;
	m_pListener->OnCrosshairStateChanged(m_bCrosshairVisible);
}

void CHUDViewModel::ShowHUD()
{
	if (m_bHUDVisible)
		return;

	m_bHUDVisible = true;
	m_bCrosshairVisible = true;
	m_pListener->OnHUDStateChanged(m_bHUDVisible);
}

void CHUDViewModel::HideHUD()
{
	if (!m_bHUDVisible)
		return;

	m_bHUDVisible = false;
	m_bCrosshairVisible = false;
	m_pListener->OnHUDStateChanged(m_bHUDVisible);
}