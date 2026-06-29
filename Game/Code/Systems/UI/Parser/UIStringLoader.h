#pragma once

#include "Languages.h"
#include "Systems/UI/UIStringTable.h"

class CUIStringLoader final
{
public:
	CUIStringLoader(const std::string& directoryPath, ELanguage lang);
	~CUIStringLoader() = default;

	bool LoadLocalization(CUIStringTable& database);

private:
	bool Parse(std::vector<char>& buffer, CUIStringTable& database);

private:
	std::string m_directoryPath;
	ELanguage m_lang;
};