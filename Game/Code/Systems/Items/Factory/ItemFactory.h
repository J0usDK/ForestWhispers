#pragma once

#include "Systems/Items/Database/ItemDatabase.h"
#include "Systems/Items/ItemInstance.h"
#include <atomic>

class CItemFactory
{
public:
	explicit CItemFactory(const CItemDatabase& database);
	~CItemFactory() = default;

	std::unique_ptr<CItemInstance> CreateInstance(TItemDefinitionID itemDefinitionID, uint32_t count = 1);
	std::unique_ptr<CItemInstance> SplitInstance(CItemInstance& sourceInstance, uint32_t countToSplit);

private:
	const CItemDatabase& m_database;
	std::atomic<uint64_t> m_idCounter;
};