#pragma once

#include "IItemLoader.h"
#include <CryString/CryString.h>

class CItemLoader final : IItemLoader
{
public:
	explicit CItemLoader(const string& directoryPath);
	~CItemLoader() = default;

	bool LoadItems(CItemDatabase& database);

private:
	string m_directoryPath;
};