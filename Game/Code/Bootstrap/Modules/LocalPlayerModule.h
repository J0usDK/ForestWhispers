#pragma once
#include <memory>
#include "Bootstrap/IBootstrapModule.h"

class CLocalPlayerService;

class CLocalPlayerModule : public IBootstrapModule
{
public:
	CLocalPlayerModule();
	~CLocalPlayerModule();

	EBootstrapPhase GetPhase() const override;
	bool Initialize() override;
	void Shutdown() override;

	CLocalPlayerService* GetService();

private:
	std::unique_ptr<CLocalPlayerService> m_pLocalPlayerService;
};