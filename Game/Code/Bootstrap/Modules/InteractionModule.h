#pragma once
#include <memory>
#include "Bootstrap/IBootstrapModule.h"

class CInteractionService;
class CLootInteractionHandler;

class CInteractionModule : public IBootstrapModule
{
public:
	CInteractionModule();
	~CInteractionModule();

	EBootstrapPhase GetPhase() const override;
	bool Initialize() override;
	void Shutdown() override;

	CInteractionService* GetService();

private:
	std::unique_ptr<CInteractionService> m_pService;
	std::unique_ptr<CLootInteractionHandler> m_pLootHandler;
};