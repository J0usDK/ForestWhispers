#pragma once
#include <vector>
#include "ILocalPlayerListener.h"

class CLocalPlayerService
{
public:
	CLocalPlayerService() = default;
	~CLocalPlayerService() = default;

	void AddListener(ILocalPlayerListener* pListener);
	void RemoveListener(ILocalPlayerListener* pListener);

	void NotifyPlayerReady(CPlayerComponent* pPlayer);
	void NotifyPlayerRemoved();

	CPlayerComponent* GetLocalPlayer() const;

private:
	std::vector<ILocalPlayerListener*> m_listeners;
	CPlayerComponent* m_pLocalPlayer = nullptr;
};