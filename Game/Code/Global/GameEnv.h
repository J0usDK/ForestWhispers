#pragma once

class CItemDatabase;
class CItemFactory;
class CPhysicalItemFactory;
class CInteractionService;

struct SGameEnvironment
{
	CItemDatabase* pItemDatabase = nullptr;
	CItemFactory* pItemFactory = nullptr;
	CPhysicalItemFactory* pPhysicalItemFactory = nullptr;

	CInteractionService* pInteractionService = nullptr;
};

extern SGameEnvironment* gGameEnv;