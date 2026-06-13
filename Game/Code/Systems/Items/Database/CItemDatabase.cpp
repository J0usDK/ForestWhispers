#include "CItemDatabase.h"

void CItemDatabase::RegisterDefinition(SItemDefinition&& def)
{
	m_itemDatabase[def.id] = std::move(def);
}

void CItemDatabase::RegisterDefinitionsBatch(std::vector<SItemDefinition>&& batch)
{
	m_itemDatabase.reserve(m_itemDatabase.size() + batch.size());
	for (auto& def : batch)
		m_itemDatabase[def.id] = std::move(def);
}

const SItemDefinition* CItemDatabase::GetItemDefinition(TItemID id) const
{
	auto it = m_itemDatabase.find(id);
	return it != m_itemDatabase.end() ? &it->second : nullptr;
}