#pragma once

class CPlayerComponent;

class ILocalPlayerListener
{
public:
	virtual ~ILocalPlayerListener() = default;

	virtual void OnLocalPlayerReady(CPlayerComponent* pPlayer) = 0;
	virtual void OnLocalPlayerRemoved() = 0;
};