#pragma once
#include <vector>
#include "UIItemData.h"

class CUIItemCollection
{
public:
	enum class ESortType { BY_TYPE, BY_NAME, BY_WEIGHT };

public:
	CUIItemCollection() = default;
	~CUIItemCollection() = default;

	void Rebuild(const std::vector<SUIItemData>& items);
	void AddItem(const SUIItemData& newItem);
	void RemoveItemAt(int itemIndex);
	void Clear();

	void ApplySort(ESortType sortType);

	const std::vector<SUIItemData>& GetFlatList() const;

private:
	void ApplySortByType();
	void ApplySortByName();
	void ApplySortByWeight();

	uint32 GetCategoryStringID(EItemType type);

private:
	struct SBlockRange
	{
		int headerIndex;
		int startIndex;
		int endIndex;

		SBlockRange() = default;
		SBlockRange(int h, int s, int e) : headerIndex(h), startIndex(s), endIndex(e) {}
	};

	std::vector<SBlockRange> m_blockRanges;
	std::vector<SUIItemData> m_data;

	ESortType m_currentSortType = ESortType::BY_TYPE;
	bool m_bSortAscending = true;
};