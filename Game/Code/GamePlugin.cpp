// Copyright 2016-2019 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "GamePlugin.h"

#include <IGameObjectSystem.h>
#include <IGameObject.h>

#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>
#include <CrySchematyc/Utils/SharedString.h>

// User libs
#include "Bootstrap/Modules/LocalizationModule.h"
#include "Bootstrap/Modules/ItemSystemModule.h"
#include "Bootstrap/Modules/InteractionModule.h"
#include "Bootstrap/Modules/LocalPlayerModule.h"
#include "Bootstrap/Modules/UISystemModule.h"
#include "Global/GameEnv.h"

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>

SGameEnvironment* gGameEnv = nullptr;

CGamePlugin::~CGamePlugin()
{
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	if (gEnv->pSchematyc)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CGamePlugin::GetCID());
	}

	if (m_pBootstrap)
	{
		m_pBootstrap->Shutdown();
		m_pBootstrap.reset();
	}

	if (gGameEnv)
	{
		delete gGameEnv;
		gGameEnv = nullptr;
	}
}

bool CGamePlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CGamePlugin");

	if (!gGameEnv)
		gGameEnv = new SGameEnvironment();

	m_pBootstrap = std::make_unique<CBootstrap>();

	m_pBootstrap->RegisterModule(EModuleID::Localization, std::make_unique<CLocalizationModule>());
	m_pBootstrap->RegisterModule(EModuleID::ItemSystem, std::make_unique<CItemSystemModule>());
	m_pBootstrap->RegisterModule(EModuleID::Interaction, std::make_unique<CInteractionModule>());
	m_pBootstrap->RegisterModule(EModuleID::LocalPlayer, std::make_unique<CLocalPlayerModule>());
	m_pBootstrap->RegisterModule(EModuleID::UISystem, std::make_unique<CUISystemModule>());

	if (!m_pBootstrap->InitializePhase(EBootstrapPhase::CoreSystems))
	{
		CryFatalError("[Bootstrap] CoreSystems initialization phase failed!");
		return false;
	}

	auto* pLocal = m_pBootstrap->GetModule<CLocalizationModule>(EModuleID::Localization);
	auto* pItems = m_pBootstrap->GetModule<CItemSystemModule>(EModuleID::ItemSystem);
	auto* pInteract = m_pBootstrap->GetModule<CInteractionModule>(EModuleID::Interaction);
	auto* pPlayer = m_pBootstrap->GetModule<CLocalPlayerModule>(EModuleID::LocalPlayer);

	gGameEnv->pUIStringTable = pLocal->GetStringTable();
	gGameEnv->pItemDatabase = pItems->GetDatabase();
	gGameEnv->pItemFactory = pItems->GetItemFactory();
	gGameEnv->pPhysicalItemFactory = pItems->GetPhysicalItemFactory();
	gGameEnv->pInteractionService = pInteract->GetService();
	gGameEnv->pLocalPlayerService = pPlayer->GetService();

	return true;
}

void CGamePlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		case ESYSTEM_EVENT_GAME_POST_INIT:
		{
			if (!m_pBootstrap->InitializePhase(EBootstrapPhase::PostInit))
				CryFatalError("[Bootstrap] PostInit initialization phase failed!");

			auto pUI = m_pBootstrap->GetModule<CUISystemModule>(EModuleID::UISystem);
			gGameEnv->pUISystem = pUI->GetUISystem();

			m_pBootstrap->Finish();
		}
		break;

		case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
		{
			auto staticAutoRegisterLambda = [](Schematyc::IEnvRegistrar& registrar)
			{ Detail::CStaticAutoRegistrar<Schematyc::IEnvRegistrar&>::InvokeStaticCallbacks(registrar); };

			if (gEnv->pSchematyc)
			{
				gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
					stl::make_unique<Schematyc::CEnvPackage>(
						CGamePlugin::GetCID(),
						"EntityComponents",
						"Crytek GmbH",
						"Components",
						staticAutoRegisterLambda
						)
				);
			}
		}
		break;
		
		case ESYSTEM_EVENT_GAME_MODE_SWITCH_START:
		{
			m_pBootstrap->OnGameModeEnter();
		}
		break;

		case ESYSTEM_EVENT_GAME_MODE_SWITCH_END:
		{
			m_pBootstrap->OnGameModeExit();
		}
		break;
	}
}

CRYREGISTER_SINGLETON_CLASS(CGamePlugin)