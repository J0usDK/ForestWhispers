#pragma once
#include <memory>
#include <vector>
#include "IBootstrapModule.h"
#include "BootstrapModuleID.h"

class CBootstrap
{
public:
	CBootstrap() = default;
	~CBootstrap() = default;

	void RegisterModule(EModuleID id, std::unique_ptr<IBootstrapModule> pModule);

	EBootstrapPhase GetCurrentPhase() const;

	template<typename TModule>
	TModule* GetModule(EModuleID id) const
	{
		IBootstrapModule* pBase = m_modules[static_cast<uint8_t>(id)].pModule.get();
		return static_cast<TModule*>(pBase);
	}

	bool InitializePhase(EBootstrapPhase phase);
	void Finish();
	void Shutdown();

private:
	struct SModuleEntry
	{
		std::unique_ptr<IBootstrapModule> pModule;
		bool bInitialized = false;
	};

	std::array<SModuleEntry, static_cast<size_t>(EModuleID::Count)> m_modules;
	EBootstrapPhase m_currentPhase = EBootstrapPhase::Uninitialized;
};