#pragma once
#include <memory>
#include "Bootstrap/IBootstrapModule.h"

class CItemDatabase;
class CItemFactory;
class CPhysicalItemFactory;

class CItemSystemModule : public IBootstrapModule
{
public:
	CItemSystemModule();
	~CItemSystemModule();

	EBootstrapPhase GetPhase() const override;
	bool Initialize() override;
	void Shutdown() override;

	CItemDatabase* GetDatabase();
	CItemFactory* GetItemFactory();
	CPhysicalItemFactory* GetPhysicalItemFactory();

private:
	bool LoadConfigs();

private:
	std::unique_ptr<CItemDatabase> m_pItemDatabase;
	std::unique_ptr<CItemFactory> m_pItemFactory;
	std::unique_ptr<CPhysicalItemFactory> m_pPhysicalItemFactory;
};