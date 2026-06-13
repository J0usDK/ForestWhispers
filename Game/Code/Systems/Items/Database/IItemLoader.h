#pragma once
#include "CItemDatabase.h"

class IItemLoader
{
public:
	virtual ~IItemLoader() = default;
	virtual bool LoadItems(CItemDatabase& database) = 0;
};