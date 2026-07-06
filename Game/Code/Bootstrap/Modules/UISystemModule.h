#pragma once
#include <memory>
#include "Bootstrap/IBootstrapModule.h"

class CUISystem;

class CUISystemModule : public IBootstrapModule
{
public:
	CUISystemModule();
	~CUISystemModule();

	EBootstrapPhase GetPhase() const override;
	bool Initialize() override;
	void Shutdown() override;

	CUISystem* GetUISystem();

private:
	std::unique_ptr<CUISystem> m_pUISystem;
};