#include "StdAfx.h"
#include "UIStringTable.h"
#include "Global/Utils/StringUtils.h"

void CUIStringTable::Reserve(uint64 size)
{
	m_table.reserve(size);
}

void CUIStringTable::Register(const char* key, const char* localizedText)
{
	UIStringID id = StringUtils::GenerateID(key);
	m_table[id] = localizedText;
}

const char* CUIStringTable::Resolve(UIStringID id) const
{
	auto it = m_table.find(id);
	if (it != m_table.end())
		return it->second;
	return "<missing string>";
}