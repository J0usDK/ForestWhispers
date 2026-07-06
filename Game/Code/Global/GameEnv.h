#pragma once

//Services
class CInteractionService;
class CLocalPlayerService;

//Items
class CItemDatabase;
class CItemFactory;
class CPhysicalItemFactory;

//UI
class CUISystem;
class CUIStringTable;

struct SGameEnvironment
{
	//Services
	CInteractionService* pInteractionService = nullptr;
	CLocalPlayerService* pLocalPlayerService = nullptr;


	//Items
	CItemDatabase* pItemDatabase = nullptr;
	CItemFactory* pItemFactory = nullptr;
	CPhysicalItemFactory* pPhysicalItemFactory = nullptr;

	//UI
	CUISystem* pUISystem = nullptr;
	CUIStringTable* pUIStringTable = nullptr;
};

extern SGameEnvironment* gGameEnv;