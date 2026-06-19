#pragma once

class CItemDatabase;
class CItemFactory;

struct SGameEnvironment
{
	CItemDatabase* pItemDatabase = nullptr;
	CItemFactory* pItemFactory = nullptr;
};

extern SGameEnvironment* gGameEnv;