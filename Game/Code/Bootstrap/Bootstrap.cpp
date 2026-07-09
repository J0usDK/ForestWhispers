#include "StdAfx.h"
#include "Bootstrap.h"

void CBootstrap::RegisterModule(EModuleID id, std::unique_ptr<IBootstrapModule> pModule)
{
	const uint8_t index = static_cast<uint8_t>(id);
	m_modules[index].pModule = std::move(pModule);
}

EBootstrapPhase CBootstrap::GetCurrentPhase() const
{
	return m_currentPhase;
}

bool CBootstrap::InitializePhase(EBootstrapPhase phase)
{
	for (auto& entry : m_modules)
	{
		if (!entry.pModule)
			continue;
		if (entry.bInitialized)
			continue;
		if (entry.pModule->GetPhase() != phase)
			continue;

		if (entry.pModule->Initialize())
			entry.bInitialized = true;
		else
			return false;
	}

	return true;
}

void CBootstrap::Finish()
{
	m_currentPhase = EBootstrapPhase::Complete;
}

void CBootstrap::Shutdown()
{
	for (int i = static_cast<int>(EModuleID::Count) - 1; i >= 0; --i)
	{
		auto& entry = m_modules[i];
		if (entry.pModule && entry.bInitialized)
		{
			entry.pModule->Shutdown();
			entry.bInitialized = false;
		}
	}
}

void CBootstrap::OnGameModeEnter()
{
	for (auto& entry : m_modules)
		entry.pModule->OnGameModeEnter();
}

void CBootstrap::OnGameModeExit()
{
	for (int i = static_cast<int>(EModuleID::Count) - 1; i >= 0; --i)
	{
		auto& entry = m_modules[i];
		if (entry.pModule && entry.bInitialized)
			entry.pModule->OnGameModeExit();
	}
}