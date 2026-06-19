#include "StdAfx.h"
#include <CrySystem/ISystem.h>
#include "ItemParser.h"
#include "Global/Utils/JsonUtils.h"

CItemParser::CItemParser()
{
	RegisterParser("condition", TraitParsers::ParseCondition);
	RegisterParser("melee_combat", TraitParsers::ParseMeleeCombat);
}

void CItemParser::RegisterParser(const std::string& jsonKey, TTraitParserFunc func)
{
	if (m_parsers.find(jsonKey) != m_parsers.end())
		CryFatalError("CItemParser: Duplicate trait key detected in registry -> %s", jsonKey.c_str());
	m_parsers[jsonKey] = func;
}

void CItemParser::Parse(std::vector<char>& buffer, SItemDefinition& outDef) const
{
	if (buffer.empty()) return;

	rapidjson::Document doc;
	doc.ParseInsitu(buffer.data());

	if (doc.HasParseError() || !doc.IsObject()) return;

	string itemID;
	if (!JsonUtils::TryGetString(doc, "ItemID", itemID))
		return;
	outDef.id = StringUtils::GenerateID(itemID);

	ParseItemTraits(doc, outDef);
}

void CItemParser::ParseItemTraits(const rapidjson::Document& doc, SItemDefinition& outDef) const
{
	if (!doc.HasMember("traits") || !doc["traits"].IsObject())
		return;

	const auto& traitsObj = doc["traits"];
	for (auto it = traitsObj.MemberBegin(); it != traitsObj.MemberEnd(); ++it)
	{
		std::string traitName = it->name.GetString();
		auto parserIt = m_parsers.find(traitName);
		if (parserIt != m_parsers.end())
			parserIt->second(it->value, outDef);
		else
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "CItemParser: Unknown trait in JSON -> %s", traitName.c_str());
	}
}