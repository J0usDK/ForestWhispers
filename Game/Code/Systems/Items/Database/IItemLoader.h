#pragma once
#include "Systems/Items/Database/ItemDatabase.h"

class IItemLoader
{
public:
	virtual ~IItemLoader() = default;
	virtual bool LoadItems(CItemDatabase& database) = 0;
};