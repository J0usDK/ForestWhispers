#include "StdAfx.h"
#include "ItemFactory.h"
#include <CrySystem/ISystem.h>

CItemFactory::CItemFactory(const CItemDatabase& database) : m_database(database)
{
	m_idCounter = 0;
}

std::unique_ptr<CItemInstance> CItemFactory::CreateInstance(TItemDefinitionID itemDefinitionID, uint32_t count)
{
	const SItemDefinition* pDef = m_database.GetItemDefinition(itemDefinitionID);
	if (!pDef)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Attempted to create item instance with unknown ID: %llu", itemDefinitionID);
		return nullptr;
	}

	TItemInstanceID newID = m_idCounter.fetch_add(1);
	CItem itemData(itemDefinitionID, StringUtils::GenerateID(pDef->name));
	for (const auto& pTraitTemplate : pDef->traits)
	{
		if (pTraitTemplate)
			itemData.AddTrait(pTraitTemplate->Clone());
	}
	return std::make_unique<CItemInstance>(std::move(itemData), newID, count);
}

std::unique_ptr<CItemInstance> CItemFactory::SplitInstance(CItemInstance& sourceInstance, uint32_t countToSplit)
{
	if (countToSplit == 0 || countToSplit >= sourceInstance.GetCount())
		return nullptr;

	sourceInstance.RemoveCount(countToSplit);

	TItemInstanceID newID = m_idCounter.fetch_add(1);
	const SItemDefinition* pDef = m_database.GetItemDefinition(sourceInstance.GetItemData().GetDefinitionID());
	CItem clonedItemData(pDef->id, StringUtils::GenerateID(pDef->name));
	sourceInstance.GetItemData().ForEachTrait([&](const IItemTrait& trait)
		{ clonedItemData.AddTrait(trait.Clone()); });
	return std::make_unique<CItemInstance>(std::move(clonedItemData), newID, countToSplit);
}