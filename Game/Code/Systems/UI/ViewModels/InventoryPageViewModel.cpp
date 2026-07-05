#include "StdAfx.h"
#include "InventoryPageViewModel.h"

#include "Global/GameEnv.h"
#include "Systems/UI/UIStringTable.h"

void CInventoryPageViewModel::SetListener(IInventoryPageViewListener* listener)
{
	m_pListener = listener;
}

void CInventoryPageViewModel::OnInventoryUpdated(const std::vector<SUIItemData>& itemsSnapshot)
{
	CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "CInventoryPageViewModel::OnInventoryUpdated: Inventory updated event received.");
	m_cachedItems = itemsSnapshot;
	ApplyCurrentSort();
	PushDataToView();
}

void CInventoryPageViewModel::OnInventoryItemAdded(const SUIItemData& newItem)
{
	CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "CInventoryPageViewModel::OnInventoryItemAdded: Inventory item added event received.");
	m_cachedItems.push_back(newItem);
	ApplyCurrentSort();
	PushDataToView();
}

void CInventoryPageViewModel::OnInventoryItemRemoved(int itemIndex)
{
	return;
}

void CInventoryPageViewModel::SortByName()
{
	m_currentSortType = ESortType::BY_NAME;
	ApplyCurrentSort();
	PushDataToView();
}

void CInventoryPageViewModel::SortByWeight()
{
	m_currentSortType = ESortType::BY_WEIGHT;
	ApplyCurrentSort();
	PushDataToView();
}

void CInventoryPageViewModel::ApplyCurrentSort()
{
	switch (m_currentSortType)
	{
		case ESortType::BY_NAME:
			std::sort(m_cachedItems.begin(), m_cachedItems.end(), [](const SUIItemData& a, const SUIItemData& b) {
				return gGameEnv->pUIStringTable->Resolve(a.nameStringID) < gGameEnv->pUIStringTable->Resolve(b.nameStringID);
			});
			break;
		case ESortType::BY_WEIGHT:
			std::sort(m_cachedItems.begin(), m_cachedItems.end(), [](const SUIItemData& a, const SUIItemData& b) {
				return a.weight > b.weight;
			});
			break;
		case ESortType::NONE:
		default:
			break;
	}
}

void CInventoryPageViewModel::PushDataToView()
{
	if (m_pListener)
		m_pListener->OnInventoryDataUpdated(m_cachedItems);
}