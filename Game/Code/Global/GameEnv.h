#pragma once

class CItemDatabase;
class CItemFactory;
class CPhysicalItemFactory;
class CInteractionService;
class CUISystem;
class CUIStringTable;

struct SGameEnvironment
{
	CItemDatabase* pItemDatabase = nullptr;
	CItemFactory* pItemFactory = nullptr;
	CPhysicalItemFactory* pPhysicalItemFactory = nullptr;

	CInteractionService* pInteractionService = nullptr;

	CUISystem* pUISystem = nullptr;

	CUIStringTable* pUIStringTable = nullptr;
};

extern SGameEnvironment* gGameEnv;