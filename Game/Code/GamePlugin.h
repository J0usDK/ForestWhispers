// Copyright 2016-2019 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryEntitySystem/IEntityClass.h>

#include "Systems/Items/Database/ItemDatabase.h"
#include "Systems/Items/Factory/ItemFactory.h"
#include "Systems/Items/Factory/PhysicalItemFactory.h"
#include "Systems/UI/UIStringTable.h"
#include "Systems/UI/UISystem.h"
#include "Services/Base/InteractionService.h"
#include "Services/Handlers/LootInteractionHandler.h"

// The entry-point of the application
// An instance of CGamePlugin is automatically created when the library is loaded
// We then construct the local player entity and CPlayerComponent instance when OnClientConnectionReceived is first called.
class CGamePlugin 
	: public Cry::IEnginePlugin
	, public ISystemEventListener
{
public:
	CRYINTERFACE_SIMPLE(Cry::IEnginePlugin)
	CRYGENERATE_SINGLETONCLASS_GUID(CGamePlugin, "Blank", "f01244b0-a4e7-4dc6-91e1-0ed18906fe7c"_cry_guid)

	virtual ~CGamePlugin();
	
	// Cry::IEnginePlugin
	virtual const char* GetCategory() const override { return "Game"; }
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~Cry::IEnginePlugin

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// Helper function to get the CGamePlugin instance
	// Note that CGamePlugin is declared as a singleton, so the CreateClassInstance will always return the same pointer
	static CGamePlugin* GetInstance()
	{
		return cryinterface_cast<CGamePlugin>(CGamePlugin::s_factory.CreateClassInstance().get());
	}
	
private:
	void InitGlobalObjects();
	void InitInteractionHandlers();
	void LoadLocalizationConfig();
	void LoadItemConfigs();

private:
	std::unique_ptr<CItemDatabase> m_pItemDatabase;
	std::unique_ptr<CItemFactory> m_pItemFactory;
	std::unique_ptr<CPhysicalItemFactory> m_pPhysicalItemFactory;
	std::unique_ptr<CUIStringTable> m_pUIStringTable;
	std::unique_ptr<CUISystem> m_pUISystem;

	std::unique_ptr<CInteractionService> m_pInteractionService;
	std::unique_ptr<CLootInteractionHandler> m_pLootService;
};