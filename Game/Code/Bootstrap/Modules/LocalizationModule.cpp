#include "StdAfx.h"
#include "LocalizationModule.h"

#include "Systems/UI/UIStringTable.h"
#include "Systems/UI/Parser/UIStringLoader.h"

CLocalizationModule::CLocalizationModule() = default;
CLocalizationModule::~CLocalizationModule() = default;

EBootstrapPhase CLocalizationModule::GetPhase() const { return EBootstrapPhase::CoreSystems; }

bool CLocalizationModule::Initialize()
{
	m_pStringTable = std::make_unique<CUIStringTable>();
	return LoadConfig();
}

bool CLocalizationModule::LoadConfig()
{
	CUIStringLoader localLoader("GameData/Locals", ELanguage::English);
	return localLoader.LoadLocalization(*m_pStringTable);
}

void CLocalizationModule::Shutdown()
{
	m_pStringTable.reset();
}

CUIStringTable* CLocalizationModule::GetStringTable() { return m_pStringTable.get(); }