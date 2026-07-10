#pragma once
#include "Systems/Items/Database/ItemTypes.h"
#include "Systems/UI/Listeners/IInventoryPageViewListener.h"
#include "Systems/UI/Listeners/IInventoryPageCommands.h"
#include "Components/Inventory/IInventoryListener.h"
#include "Systems/UI/Types/UIItemCollection.h"

class CInventoryPageViewModel : public IInventoryListener, public IInventoryPageCommands
{
public:
	CInventoryPageViewModel() = default;
	~CInventoryPageViewModel() = default;

	void Reset();
	void SetListener(IInventoryPageViewListener* listener);

	void OnInventoryUpdated(const std::vector<SUIItemData>& itemsSnapshot) override;
	void OnInventoryItemAdded(const SUIItemData& newItem) override;
	void OnInventoryItemRemoved(int itemIndex) override;

	void SortByType() override;
	void SortByName() override;
	void SortByWeight() override;

private:
	void PushDataToView();

private:
	IInventoryPageViewListener* m_pListener = nullptr;
	bool m_bIsActive = false;

	CUIItemCollection m_cachedItemsData;
};