#include "StdAfx.h"
#include "ItemSystemModule.h"

#include "Systems/Items/Database/ItemDatabase.h"
#include "Systems/Items/Factory/ItemFactory.h"
#include "Systems/Items/Factory/PhysicalItemFactory.h"
#include "Systems/Items/Database/ItemLoader.h"
#include "Systems/Items/Parser/ItemParser.h"

CItemSystemModule::CItemSystemModule() = default;
CItemSystemModule::~CItemSystemModule() = default;

EBootstrapPhase CItemSystemModule::GetPhase() const { return EBootstrapPhase::CoreSystems; }

bool CItemSystemModule::Initialize()
{
	m_pItemDatabase = std::make_unique<CItemDatabase>();
	m_pItemFactory = std::make_unique<CItemFactory>(*m_pItemDatabase);
	m_pPhysicalItemFactory = std::make_unique<CPhysicalItemFactory>();

	return LoadConfigs();
}

bool CItemSystemModule::LoadConfigs()
{
	CItemParser itemParser;
	CItemLoader itemLoader("GameData/Items", itemParser);

	return itemLoader.LoadItems(*m_pItemDatabase);
}

void CItemSystemModule::Shutdown()
{
	m_pPhysicalItemFactory.reset();
	m_pItemFactory.reset();
	m_pItemDatabase.reset();
}

CItemDatabase* CItemSystemModule::GetDatabase() { return m_pItemDatabase.get(); }
CItemFactory* CItemSystemModule::GetItemFactory() { return m_pItemFactory.get(); }
CPhysicalItemFactory* CItemSystemModule::GetPhysicalItemFactory() { return m_pPhysicalItemFactory.get(); }
