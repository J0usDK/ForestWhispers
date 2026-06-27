#pragma once
#include "Systems/Items/ItemInstance.h"

class IInventory
{
public:
	virtual ~IInventory() = default;

	virtual bool CanAddItem(const CItemInstance* pInstance) const = 0;
	virtual bool TryAddItem(std::unique_ptr<CItemInstance>& pInstance) = 0;
	virtual std::unique_ptr<CItemInstance> RemoveItem(TItemInstanceID instanceID, uint32_t count) = 0;

	virtual const CItemInstance* GetItem(TItemInstanceID instanceID) const = 0;
	virtual const CItemInstance* GetItemAt(uint32_t index) const = 0;

	virtual uint32_t GetTotalCountOf(TItemDefinitionID definitionID) const = 0;

	virtual uint32_t GetCurrentItemCount() const = 0;
	virtual uint32_t GetMaxSlots() const = 0;
};