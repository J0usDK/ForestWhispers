#pragma once
#include <unordered_map>
#include <CryString/CryString.h>

using UIStringID = uint32;

class CUIStringTable
{
public:
	CUIStringTable() = default;
	~CUIStringTable() = default;

	void Reserve(uint32 size);
	void Register(const char* key, const char* localizedText);
	const char* Resolve(UIStringID id) const;

private:
	std::unordered_map<UIStringID, string> m_table;
};