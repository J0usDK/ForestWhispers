#pragma once
#include "Systems/UI/Listeners/IInventoryPageViewListener.h"
#include "Components/Inventory/IInventoryListener.h"

class CInventoryPageViewModel : public IInventoryListener
{
public:
	CInventoryPageViewModel() = default;
	~CInventoryPageViewModel() = default;


	void SetListener(IInventoryPageViewListener* listener);

	void OnInventoryUpdated(const std::vector<SUIItemData>& itemsSnapshot) override;
	void OnInventoryItemAdded(const SUIItemData& newItem) override;
	void OnInventoryItemRemoved(int itemIndex) override;

	void SortByName();
	void SortByWeight();

private:
	void ApplyCurrentSort();
	void PushDataToView();

private:
	IInventoryPageViewListener* m_pListener = nullptr;
	bool m_bIsActive = false;

	std::vector<SUIItemData> m_cachedItems;

	enum class ESortType
	{
		NONE,
		BY_NAME,
		BY_WEIGHT
	} m_currentSortType = ESortType::NONE;
};