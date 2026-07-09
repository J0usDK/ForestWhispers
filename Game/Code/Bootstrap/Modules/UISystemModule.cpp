#include "StdAfx.h"
#include "UISystemModule.h"

#include "Global/GameEnv.h"
#include "Services/Base/LocalPlayer/LocalPlayerService.h"
#include "Systems/UI/UISystem.h"

CUISystemModule::CUISystemModule() = default;
CUISystemModule::~CUISystemModule() = default;

EBootstrapPhase CUISystemModule::GetPhase() const { return EBootstrapPhase::PostInit; }

bool CUISystemModule::Initialize()
{
	m_pUISystem = std::make_unique<CUISystem>();
	gGameEnv->pLocalPlayerService->AddListener(m_pUISystem.get());
	return true;
}

void CUISystemModule::Shutdown()
{
	gGameEnv->pLocalPlayerService->RemoveListener(m_pUISystem.get());
	m_pUISystem.reset();
}

void CUISystemModule::OnGameModeEnter()
{
	m_pUISystem->OnGameModeEnter();
}

void CUISystemModule::OnGameModeExit()
{
	m_pUISystem->OnGameModeExit();
}

CUISystem* CUISystemModule::GetUISystem() { return m_pUISystem.get(); }