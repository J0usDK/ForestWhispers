#include "StdAfx.h"
#include "LocalPlayerService.h"

void CLocalPlayerService::AddListener(ILocalPlayerListener* pListener)
{
	if (!pListener) return;

	if (std::find(m_listeners.begin(), m_listeners.end(), pListener) == m_listeners.end())
		m_listeners.emplace_back(pListener);
}

void CLocalPlayerService::RemoveListener(ILocalPlayerListener* pListener)
{
	m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), pListener), m_listeners.end());
}

void CLocalPlayerService::NotifyPlayerReady(CPlayerComponent* pPlayer)
{
	m_pLocalPlayer = pPlayer;
	for (auto* listener : m_listeners)
		listener->OnLocalPlayerReady(pPlayer);
}

void CLocalPlayerService::NotifyPlayerRemoved()
{
	m_pLocalPlayer = nullptr;
	for (auto* listener : m_listeners)
		listener->OnLocalPlayerRemoved();
}

CPlayerComponent* CLocalPlayerService::GetLocalPlayer() const { return m_pLocalPlayer; }