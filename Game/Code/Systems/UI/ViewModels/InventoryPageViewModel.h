#pragma once
#include "Systems/UI/Listeners/IInventoryPageViewListener.h"
#include "Systems/UI/Listeners/IInventoryPageCommands.h"
#include "Components/Inventory/IInventoryListener.h"

class CInventoryPageViewModel : public IInventoryListener, public IInventoryPageCommands
{
public:
	CInventoryPageViewModel() = default;
	~CInventoryPageViewModel() = default;


	void SetListener(IInventoryPageViewListener* listener);

	void OnInventoryUpdated(const std::vector<SUIItemData>& itemsSnapshot) override;
	void OnInventoryItemAdded(const SUIItemData& newItem) override;
	void OnInventoryItemRemoved(int itemIndex) override;

	void SortByType() override;
	void SortByName() override;
	void SortByWeight() override;

private:
	void RebuildList(const std::vector<SUIItemData>& itemsSnapshot);
	void PushDataToView();
	uint32 GetCategoryStringID(ItemType type);

private:
	IInventoryPageViewListener* m_pListener = nullptr;
	bool m_bIsActive = false;

	enum class ESortType
	{
		BY_TYPE,
		BY_NAME,
		BY_WEIGHT
	} m_currentSortType = ESortType::BY_TYPE;

	struct SBlockRange
	{
		int headerIndex;
		int startIndex;
		int endIndex;
	};

	std::vector<SUIItemData> m_cachedItems;
	std::vector<SBlockRange> m_blockRanges;
};