#pragma once
#include <memory>
#include "Bootstrap/IBootstrapModule.h"

class CUIStringTable;

class CLocalizationModule : public IBootstrapModule
{
public:
	CLocalizationModule();
	~CLocalizationModule();

	EBootstrapPhase GetPhase() const override;
	bool Initialize() override;
	void Shutdown() override;
	
	CUIStringTable* GetStringTable();

private:
	bool LoadConfig();

private:
	std::unique_ptr<CUIStringTable> m_pStringTable;
};