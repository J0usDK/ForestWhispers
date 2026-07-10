#include "StdAfx.h"
#include "ItemInstance.h"
#include <cassert>

CItemInstance::CItemInstance(CItem&& itemData, TItemInstanceID instanceID, uint32_t count)
	: m_pItemData(std::move(itemData)), m_instanceID(instanceID), m_count(count)
{
}

TItemInstanceID CItemInstance::GetInstanceID() const
{
	return m_instanceID;
}

uint32_t CItemInstance::GetCount() const
{
	return m_count;
}

float CItemInstance::GetWeight() const
{
	return m_pItemData.GetDefinition()->weight * m_count;
}

void CItemInstance::AddCount(uint32_t count)
{
	m_count += count;
}

void CItemInstance::RemoveCount(uint32_t count)
{
	if (m_count >= count) m_count -= count;
}

const CItem& CItemInstance::GetItemData() const
{
	return m_pItemData;
}

CItem& CItemInstance::GetItemData()
{
	return m_pItemData;
}

bool CItemInstance::CanUnite(const CItemInstance& pOther) const
{
	return m_pItemData.IsEqual(pOther.GetItemData());
}