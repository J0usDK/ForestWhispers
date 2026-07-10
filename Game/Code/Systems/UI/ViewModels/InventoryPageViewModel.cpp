#include "StdAfx.h"
#include "InventoryPageViewModel.h"

#include "Systems/Items/Database/ItemTypes.h"
#include "Systems/UI/Types/UIItemData.h"
#include "Global/GameEnv.h"
#include "Global/Utils/StringUtils.h"
#include "Systems/UI/UIStringTable.h"

void CInventoryPageViewModel::SetListener(IInventoryPageViewListener* listener)
{
	m_pListener = listener;
}

void CInventoryPageViewModel::Reset()
{
	m_bIsActive = false;
	m_cachedItemsData.Clear();
}

void CInventoryPageViewModel::OnInventoryUpdated(const std::vector<SUIItemData>& itemsSnapshot)
{
	m_cachedItemsData.Rebuild(itemsSnapshot);
	PushDataToView();
}

void CInventoryPageViewModel::OnInventoryItemAdded(const SUIItemData& newItem)
{
	m_cachedItemsData.AddItem(newItem);
	PushDataToView();
}

void CInventoryPageViewModel::OnInventoryItemRemoved(int itemIndex)
{
	m_cachedItemsData.RemoveItemAt(itemIndex);
	return;
}

void CInventoryPageViewModel::SortByType()
{
	m_cachedItemsData.ApplySort(CUIItemCollection::ESortType::BY_TYPE);
	PushDataToView();
}

void CInventoryPageViewModel::SortByName()
{
	m_cachedItemsData.ApplySort(CUIItemCollection::ESortType::BY_NAME);
	PushDataToView();
}

void CInventoryPageViewModel::SortByWeight()
{
	m_cachedItemsData.ApplySort(CUIItemCollection::ESortType::BY_WEIGHT);
	PushDataToView();
}

void CInventoryPageViewModel::PushDataToView()
{
	if (m_pListener)
		m_pListener->OnInventoryDataUpdated(m_cachedItemsData.GetFlatList());
}