#pragma once

class CItemDatabase;
class CItemFactory;
class CPhysicalItemFactory;

struct SGameEnvironment
{
	CItemDatabase* pItemDatabase = nullptr;
	CItemFactory* pItemFactory = nullptr;
	CPhysicalItemFactory* pPhysicalItemFactory = nullptr;
};

extern SGameEnvironment* gGameEnv;