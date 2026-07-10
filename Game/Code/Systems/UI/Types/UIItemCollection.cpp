#include "StdAfx.h"
#include "UIItemCollection.h"

#include "Global/Utils/StringUtils.h"
#include "Global/GameEnv.h"
#include "Systems/UI/UIStringTable.h"

namespace
{
	constexpr uint32 CATEGORY_HASHES[] = {
		StringUtils::GenerateID("ui_inv_type_swords"),
		StringUtils::GenerateID("ui_inv_type_food")
	};

	static_assert(std::size(CATEGORY_HASHES) == static_cast<size_t>(EItemType::Count),
		"CATEGORY_HASHES array size does not match EItemType count of types");
}

void CUIItemCollection::Clear()
{
	m_data.clear();
	m_blockRanges.clear();
}

void CUIItemCollection::Rebuild(const std::vector<SUIItemData>& items)
{
	Clear();
	if (items.empty()) return;

	std::vector<SUIItemData> groupedItems = items;
	std::sort(groupedItems.begin(), groupedItems.end(), [](const SUIItemData& a, const SUIItemData& b) {
		return a.itemType < b.itemType;
	});

	size_t countOfHeaders = static_cast<size_t>(EItemType::Count);
	m_data.reserve(groupedItems.size() + countOfHeaders);
	m_blockRanges.reserve(countOfHeaders);

	EItemType lastType = (EItemType)-1;
	for (auto& item : groupedItems)
	{
		if (item.itemType != lastType)
		{
			int size = m_data.size();

			m_data.emplace_back(GetCategoryStringID(item.itemType), 0.0f, item.itemType, UITypes::ROW_HEADER);
			m_blockRanges.emplace_back(size, size + 1, size + 1);
			lastType = item.itemType;
		}
		m_data.push_back(std::move(item));
	}
}

void CUIItemCollection::AddItem(const SUIItemData& newItem)
{
	auto it = std::find_if(m_blockRanges.begin(), m_blockRanges.end(), [&](const SBlockRange& range) {
		return m_data[range.headerIndex].itemType == newItem.itemType;
	});

	if (it != m_blockRanges.end())
	{
		auto insertPos = m_data.begin() + it->endIndex;
		m_data.insert(insertPos, newItem);

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
		int size = m_data.size();

		m_data.emplace_back(GetCategoryStringID(newItem.itemType), 0.0f, newItem.itemType, UITypes::ROW_HEADER);
		m_data.push_back(newItem);
		m_blockRanges.emplace_back(size, size + 1, size + 2);
	}
}

void CUIItemCollection::RemoveItemAt(int itemIndex)
{
	return;
}

void CUIItemCollection::ApplySort(ESortType sortType)
{
	if (m_blockRanges.empty() || m_data.empty()) return;

	if (m_currentSortType == sortType)
		m_bSortAscending = !m_bSortAscending;
	else
		m_currentSortType = sortType;

	switch (sortType)
	{
		case ESortType::BY_TYPE:
			ApplySortByType();
			break;
		case ESortType::BY_NAME:
			ApplySortByName();
			break;
		case ESortType::BY_WEIGHT:
			ApplySortByWeight();
			break;
	}
}

void CUIItemCollection::ApplySortByType()
{
	std::sort(m_blockRanges.begin(), m_blockRanges.end(), [this](const SBlockRange& a, const SBlockRange& b) {
		auto typeA = m_data[a.headerIndex].itemType;
		auto typeB = m_data[b.headerIndex].itemType;
		return m_bSortAscending ? (typeA < typeB) : (typeA > typeB);
	});

	std::vector<SUIItemData> reordered;
	reordered.reserve(m_data.size());

	for (auto& block : m_blockRanges)
	{
		int oldHeaderIdx = block.headerIndex;
		block.headerIndex = reordered.size();

		reordered.push_back(std::move(m_data[oldHeaderIdx]));

		block.startIndex = reordered.size();
		for (int i = oldHeaderIdx + 1; i < block.endIndex; ++i)
			reordered.push_back(std::move(m_data[i]));
		block.endIndex = reordered.size();
	}
	m_data = std::move(reordered);
}

void CUIItemCollection::ApplySortByName()
{
	for (const auto& block : m_blockRanges)
	{
		auto start = m_data.begin() + block.startIndex;
		auto end = m_data.begin() + block.endIndex;

		std::sort(start, end, [this](const SUIItemData& a, const SUIItemData& b) {
			std::string_view nameA = gGameEnv->pUIStringTable->Resolve(a.nameStringID);
			std::string_view nameB = gGameEnv->pUIStringTable->Resolve(b.nameStringID);
			return m_bSortAscending ? (nameA < nameB) : (nameA > nameB);
		});
	}
}

void CUIItemCollection::ApplySortByWeight()
{
	for (const auto& block : m_blockRanges)
	{
		auto start = m_data.begin() + block.startIndex;
		auto end = m_data.begin() + block.endIndex;

		std::sort(start, end, [this](const SUIItemData& a, const SUIItemData& b) {
			return m_bSortAscending ? (a.weight < b.weight) : (a.weight > b.weight);
		});
	}
}

uint32 CUIItemCollection::GetCategoryStringID(EItemType type)
{
	const size_t index = static_cast<size_t>(type);
	if (index < std::size(CATEGORY_HASHES))
		return CATEGORY_HASHES[index];
	return 0;
}

const std::vector<SUIItemData>& CUIItemCollection::GetFlatList() const
{
	return m_data;
}