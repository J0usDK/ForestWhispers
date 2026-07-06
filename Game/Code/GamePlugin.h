// Copyright 2016-2019 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryEntitySystem/IEntityClass.h>

#include "Bootstrap/Bootstrap.h"

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
	
	virtual const char* GetCategory() const override { return "Game"; }
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;

	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;

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
	std::unique_ptr<CBootstrap> m_pBootstrap;
};