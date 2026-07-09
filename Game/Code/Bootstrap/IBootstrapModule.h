#pragma once

enum class EBootstrapPhase
{
	Uninitialized,
	CoreSystems,
	PostInit,
	Complete,
	Count
};

class IBootstrapModule
{
public:
	virtual EBootstrapPhase GetPhase() const = 0;
	virtual bool Initialize() = 0;
	virtual void Shutdown() {};

	virtual void OnGameModeEnter() {};
	virtual void OnGameModeExit() {};
};