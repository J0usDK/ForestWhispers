#include "StdAfx.h"
#include "LocalPlayerModule.h"

#include "Services/Base/LocalPlayer/LocalPlayerService.h"

CLocalPlayerModule::CLocalPlayerModule() = default;
CLocalPlayerModule::~CLocalPlayerModule() = default;

EBootstrapPhase CLocalPlayerModule::GetPhase() const { return EBootstrapPhase::CoreSystems; }

bool CLocalPlayerModule::Initialize()
{
	m_pLocalPlayerService = std::make_unique<CLocalPlayerService>();
	return true;
}

void CLocalPlayerModule::Shutdown()
{
	m_pLocalPlayerService.reset();
}

CLocalPlayerService* CLocalPlayerModule::GetService() { return m_pLocalPlayerService.get(); }