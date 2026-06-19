#pragma once
#include "Systems/Items/Item.h"

using TItemInstanceID = uint64_t;

class CItemInstance
{
public:
	CItemInstance(CItem&& itemData, TItemInstanceID instanceID, uint32_t count = 1);

	TItemInstanceID GetInstanceID() const;
	uint32_t GetCount() const;

	void AddCount(uint32_t amount);
	void RemoveCount(uint32_t amount);

	bool CanUnite(const CItemInstance& pOther) const;
	const CItem& GetItemData() const;
	CItem& GetItemData();

private:
	TItemInstanceID m_instanceID;
	uint32_t m_count;

	CItem m_pItemData;
};