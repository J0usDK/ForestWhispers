#pragma once

#include <CryEntitySystem/IEntityComponent.h>

#include "IInventory.h"
#include "IInventoryEventSender.h"
#include "IInventoryListener.h"

class CInventoryComponent final : public IEntityComponent, public IInventory, public IInventoryEventSender
{
public:
	CInventoryComponent() = default;
	virtual ~CInventoryComponent() = default;

	CInventoryComponent(const CInventoryComponent&) = delete;
	CInventoryComponent& operator=(const CInventoryComponent&) = delete;

	void RegisterListener(IInventoryListener* pListener) override;
	void UnregisterListener(IInventoryListener* pListener) override;

	static void ReflectType(Schematyc::CTypeDesc<CInventoryComponent>& desc)
	{
		desc.SetGUID("{FF853761-BA2C-4387-B4AA-59850A5E7D09}"_cry_guid);

		desc.SetEditorCategory("RPG Systems");
		desc.SetLabel("Inventory");
		desc.SetDescription("Container for items (Player, NPCs, Chests, etc.)");

		desc.AddMember(&CInventoryComponent::m_maxSlots, 'maxs', "MaxSlots", "Maximum Slots", "How many items this inventory can hold", 0u);
	}

	bool CanAddItem(const CItemInstance* pInstance) const override;
	bool TryAddItem(std::unique_ptr<CItemInstance>& pInstance) override;
	std::unique_ptr<CItemInstance> RemoveItem(TItemInstanceID instanceID, uint32_t count) override;

	const CItemInstance* GetItem(TItemInstanceID instanceID) const override;
	const CItemInstance* GetItemAt(uint32_t index) const override;

	uint32_t GetTotalCountOf(TItemDefinitionID definitionID) const override;
	uint32_t GetCurrentItemCount() const override;
	uint32_t GetMaxSlots() const override;

private:
	void NotifyItemAdded(const SUIItemData& itemData);

private:
	std::vector<IInventoryListener*> m_listeners;
	std::vector<std::unique_ptr<CItemInstance>> m_instances;

	uint32_t m_maxSlots = 0u;
};