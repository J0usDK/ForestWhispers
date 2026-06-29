#include "StdAfx.h"
#include "UIStringLoader.h"

#include <filesystem>

#include "Global/FileSystem/ConfigReader.h"
#include "Global/Utils/JsonUtils.h"

CUIStringLoader::CUIStringLoader(const std::string& directoryPath, ELanguage lang)
{
	m_directoryPath = directoryPath;
	m_lang = lang;
}

bool CUIStringLoader::LoadLocalization(CUIStringTable& database)
{
	std::filesystem::path filePath = std::filesystem::path(m_directoryPath) / std::string(LocalNames::Table[(size_t)m_lang]);
	std::string tmpPath = filePath.string();

	std::vector<char> buffer = CConfigReader::ReadFile(tmpPath.c_str());

	return Parse(buffer, database);
}

bool CUIStringLoader::Parse(std::vector<char>& buffer, CUIStringTable& database)
{
	if (buffer.empty()) return false;

	rapidjson::Document doc;
	doc.ParseInsitu(buffer.data());

	if (doc.HasParseError() || !doc.IsObject()) return false;

	database.Reserve(doc.MemberCount());
	for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
	{
		if (it->name.IsString() && it->value.IsString())
			database.Register(it->name.GetString(), it->value.GetString());
	}

	return true;
}