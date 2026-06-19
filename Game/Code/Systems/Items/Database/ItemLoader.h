#pragma once

#include "Systems/Items/Database/IItemLoader.h"
#include "Systems/Items/Parser/ItemParser.h"
#include <CryString/CryString.h>

class CItemLoader final : public IItemLoader
{
public:
	explicit CItemLoader(const string& directoryPath, const CItemParser& parser);
	~CItemLoader() = default;

	bool LoadItems(CItemDatabase& database);

private:
	string m_directoryPath;
	const CItemParser& m_parser;
};