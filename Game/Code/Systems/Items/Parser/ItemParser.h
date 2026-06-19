#pragma once
#include <unordered_map>
#include "Systems/Items/Parser/TraitParsers.h"

class CItemParser final
{
public:
	CItemParser();
	~CItemParser() = default;

	void Parse(std::vector<char>& buffer, SItemDefinition& outDef) const;
	void ParseItemTraits(const rapidjson::Document& doc, SItemDefinition& outDef) const;

private:
	void RegisterParser(const std::string& jsonKey, TTraitParserFunc func);

private:
	std::unordered_map<std::string, TTraitParserFunc> m_parsers;
};