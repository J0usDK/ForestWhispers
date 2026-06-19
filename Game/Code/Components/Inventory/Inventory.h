#pragma once
#include "IInventory.h"

class CInventory final : public IInventory
{
public:
	explicit CInventory(uint32_t maxSlots);
	~CInventory() override = default;

	bool CanAddItem(const CItemInstance* pInstance) const override;
	bool AddItem(std::unique_ptr<CItemInstance> pInstance) override;
	std::unique_ptr<CItemInstance> RemoveItem(TItemInstanceID instanceID, uint32_t count) override;

	const CItemInstance* GetItem(TItemInstanceID instanceID) const override;
	const CItemInstance* GetItemAt(uint32_t index) const override;

	uint32_t GetTotalCountOf(TItemDefinitionID definitionID) const override;
	uint32_t GetCurrentItemCount() const override;
	uint32_t GetMaxSlots() const override;

private:
	std::vector<std::unique_ptr<CItemInstance>> m_instances;

	uint32_t m_maxSlots;
};