#include "StdAfx.h"
#include "InventoryComponent.h"
#include "Global/GameEnv.h"
#include "Systems/Items/Factory/ItemFactory.h"

CInventoryComponent::CInventoryComponent(uint32_t maxSlots) : m_maxSlots(maxSlots)
{
}

bool CInventoryComponent::CanAddItem(const CItemInstance* pInstance) const
{
	if (!pInstance) return false;
	for (const auto& instance : m_instances)
		if (instance->CanUnite(*pInstance)) return true;

	if (m_instances.size() >= m_maxSlots) return false;
	return true;
}

bool CInventoryComponent::TryAddItem(std::unique_ptr<CItemInstance> pInstance)
{
	if (!pInstance) return false;
	for (const auto& instance : m_instances)
	{
		if (instance->CanUnite(*pInstance))
		{
			instance->AddCount(pInstance->GetCount());
			return true;
		}
	}
	if (m_instances.size() >= m_maxSlots) return false;
	m_instances.push_back(std::move(pInstance));
	return true;
}

std::unique_ptr<CItemInstance> CInventoryComponent::RemoveItem(TItemInstanceID instanceID, uint32_t count)
{
	auto it = std::find_if(m_instances.begin(), m_instances.end(),
		[instanceID](const std::unique_ptr<CItemInstance>& item)
		{ return item->GetInstanceID() == instanceID; });

	if (it == m_instances.end() || count == 0) return nullptr;

	CItemInstance* pSource = it->get();
	count = std::min(count, pSource->GetCount());

	if (count == pSource->GetCount())
	{
		std::unique_ptr<CItemInstance> extracted = std::move(*it);
		std::swap(*it, m_instances.back());
		m_instances.pop_back();
		return extracted;
	}

	return gGameEnv->pItemFactory->SplitInstance(*pSource, count);
}

const CItemInstance* CInventoryComponent::GetItem(TItemInstanceID instanceID) const
{
	auto it = std::find_if(m_instances.begin(), m_instances.end(),
		[instanceID](const std::unique_ptr<CItemInstance>& item)
		{ return item->GetInstanceID() == instanceID; });
	
	return (it != m_instances.end()) ? it->get() : nullptr;
}

const CItemInstance* CInventoryComponent::GetItemAt(uint32_t index) const
{
	if (index < m_instances.size())
		return m_instances[index].get();
	return nullptr;
}

uint32_t CInventoryComponent::GetTotalCountOf(TItemDefinitionID definitionID) const
{
	uint32_t total = 0;
	for (const auto& instance : m_instances)
	{
		if (instance->GetItemData().GetDefinitionID() == definitionID)
			total += instance->GetCount();
	}
	return total;
}

uint32_t CInventoryComponent::GetCurrentItemCount() const
{
	return static_cast<uint32_t>(m_instances.size());
}

uint32_t CInventoryComponent::GetMaxSlots() const
{
	return m_maxSlots;
}