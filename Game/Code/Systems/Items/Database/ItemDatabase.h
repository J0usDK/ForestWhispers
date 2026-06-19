#pragma once

#include <unordered_map>
#include "Systems/Items/Database/ItemDefinition.h"

class CItemDatabase
{
public:
	CItemDatabase() = default;

	void RegisterDefinition(SItemDefinition&& def);
	void RegisterDefinitionsBatch(std::vector<SItemDefinition>&& batch);
	const SItemDefinition* GetItemDefinition(TItemDefinitionID id) const;

private:
	std::unordered_map<TItemDefinitionID, SItemDefinition> m_itemDatabase;
};