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
	m_cachedItems.clear();
	m_blockRanges.clear();

	m_bIsActive = false;
	m_currentSortType = ESortType::BY_TYPE;
}

void CInventoryPageViewModel::OnInventoryUpdated(const std::vector<SUIItemData>& itemsSnapshot)
{
	RebuildList(itemsSnapshot);
	PushDataToView();
}

void CInventoryPageViewModel::OnInventoryItemAdded(const SUIItemData& newItem)
{
	auto it = std::find_if(m_blockRanges.begin(), m_blockRanges.end(), [&](const SBlockRange& r) {
		return m_cachedItems[r.headerIndex].itemType == newItem.itemType;
	});

	if (it != m_blockRanges.end())
	{
		auto insertPos = m_cachedItems.begin() + it->endIndex;
		m_cachedItems.insert(insertPos, newItem);

		it->endIndex++;
		for (auto nextIt = it + 1; nextIt != m_blockRanges.end(); ++nextIt)
		{
			nextIt->headerIndex++;
			nextIt->startIndex++;
			nextIt->endIndex++;
		}
	}
	else
	{
		SBlockRange range;
		range.headerIndex = m_cachedItems.size();
		range.startIndex = range.headerIndex + 1;
		range.endIndex = range.headerIndex + 2;

		SUIItemData header;
		header.itemType = newItem.itemType;
		header.rowType = UITypes::ROW_HEADER;
		header.weight = 0;
		header.nameStringID = GetCategoryStringID(header.itemType);

		m_blockRanges.push_back(range);
		m_cachedItems.push_back(header);
		m_cachedItems.push_back(newItem);
	}

	PushDataToView();
}

void CInventoryPageViewModel::OnInventoryItemRemoved(int itemIndex)
{
	return;
}

void CInventoryPageViewModel::SortByType()
{
	m_currentSortType = ESortType::BY_TYPE;
	
	std::sort(m_blockRanges.begin(), m_blockRanges.end(), [&](const SBlockRange& a, const SBlockRange& b) {
		return m_cachedItems[a.headerIndex].itemType < m_cachedItems[b.headerIndex].itemType;
	});

	std::vector<SUIItemData> reordered;
	reordered.reserve(m_cachedItems.size());

	for (auto& block : m_blockRanges)
	{
		int oldHeaderIdx = block.headerIndex;
		block.headerIndex = reordered.size();
		reordered.push_back(m_cachedItems[oldHeaderIdx]);

		block.startIndex = reordered.size();
		for (int i = oldHeaderIdx + 1; i < block.endIndex; ++i)
			reordered.push_back(m_cachedItems[i]);
		block.endIndex = reordered.size();
	}
	m_cachedItems = std::move(reordered);

	PushDataToView();
}

void CInventoryPageViewModel::SortByName()
{
	m_currentSortType = ESortType::BY_NAME;

	for (const auto& block : m_blockRanges)
	{
		auto start = m_cachedItems.begin() + block.startIndex;
		auto end = m_cachedItems.begin() + block.endIndex;

		std::sort(start, end, [](const SUIItemData& a, const SUIItemData& b) {
			return gGameEnv->pUIStringTable->Resolve(a.nameStringID) < gGameEnv->pUIStringTable->Resolve(b.nameStringID);
		});
	}

	PushDataToView();
}

void CInventoryPageViewModel::SortByWeight()
{
	m_currentSortType = ESortType::BY_WEIGHT;

	for (const auto& block : m_blockRanges)
	{
		auto start = m_cachedItems.begin() + block.startIndex;
		auto end = m_cachedItems.begin() + block.endIndex;

		std::sort(start, end, [](const SUIItemData& a, const SUIItemData& b) {
			return a.weight > b.weight;
		});
	}

	PushDataToView();
}

uint32 CInventoryPageViewModel::GetCategoryStringID(EItemType type)
{
	switch (type)
	{
		case EItemType::Sword:
			return StringUtils::GenerateID("ui_inv_type_swords");
		case EItemType::Food:
			return StringUtils::GenerateID("ui_inv_type_food");
		case EItemType::None:
		default:
			return 0;
	}
}

void CInventoryPageViewModel::RebuildList(const std::vector<SUIItemData>& itemsSnapshot)
{
	m_cachedItems.clear();
	m_blockRanges.clear();

	auto m_sortedItems = itemsSnapshot;

	std::sort(m_sortedItems.begin(), m_sortedItems.end(), [](const SUIItemData& a, const SUIItemData& b) {
		return a.itemType < b.itemType;
	});

	m_cachedItems.reserve(m_sortedItems.size() + 10);
	m_blockRanges.reserve(10);

	EItemType lastType = (EItemType)-1;
	for (const auto& item : m_sortedItems)
	{
		if (item.itemType != lastType)
		{
			SBlockRange range;
			range.headerIndex = m_cachedItems.size();
			range.startIndex = m_cachedItems.size() + 1;

			SUIItemData header;
			header.rowType = UITypes::ROW_HEADER;
			header.itemType = item.itemType;
			header.weight = 0;
			header.nameStringID = GetCategoryStringID(item.itemType);
			m_cachedItems.push_back(header);

			m_blockRanges.push_back(range);
			lastType = item.itemType;
		}

		m_cachedItems.push_back(item);
		m_blockRanges.back().endIndex = m_cachedItems.size();
	}
}

void CInventoryPageViewModel::PushDataToView()
{
	if (m_pListener)
		m_pListener->OnInventoryDataUpdated(m_cachedItems);
}