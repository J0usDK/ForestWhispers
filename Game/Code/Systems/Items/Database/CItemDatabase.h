#pragma once

#include <string>
#include <unordered_map>
#include "Systems/Items/Item.h"

struct SItemDefinition
{
	TItemID id = 0;
	std::string name;
	std::vector<std::unique_ptr<IItemTrait>> traits;
};

class CItemDatabase
{
public:
	CItemDatabase() = default;

	void RegisterDefinition(SItemDefinition&& def);
	void RegisterDefinitionsBatch(std::vector<SItemDefinition>&& batch);
	const SItemDefinition* GetItemDefinition(TItemID id) const;

private:
	std::unordered_map<TItemID, SItemDefinition> m_itemDatabase;
};