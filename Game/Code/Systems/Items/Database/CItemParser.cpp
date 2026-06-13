#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include "CItemParser.h"

#include "Systems/Items/Traits/TConditionTrait.h"
#include "Systems/Items/Traits/TMeleeCombatTrait.h"
#include <Includes/rapidjson/document.h>

namespace
{
	void ParseConditionTrait(const rapidjson::Value& traitsBuffer, SItemDefinition& outDef)
	{
		auto pCondition = std::make_unique<TConditionTrait>();
		const auto& conditionData = traitsBuffer["condition"];

		if (conditionData.HasMember("maxDurability") && conditionData["maxDurability"].IsNumber())
			pCondition->maxDurability = conditionData["maxDurability"].GetFloat();

		if (conditionData.HasMember("currentDurability") && conditionData["currentDurability"].IsNumber())
			pCondition->currentDurability = conditionData["currentDurability"].GetFloat();

		outDef.traits.push_back(std::move(pCondition));
	}

	void ParseMeleeCombatTrait(const rapidjson::Value& traitsBuffer, SItemDefinition& outDef)
	{
		auto pMelee = std::make_unique<TMeleeCombatTrait>();
		const auto& meleeData = traitsBuffer["melee_combat"];

		if (meleeData.HasMember("baseSlashingDamage") && meleeData["baseSlashingDamage"].IsNumber())
			pMelee->baseSlashingDamage = meleeData["baseSlashingDamage"].GetFloat();

		if (meleeData.HasMember("basePiercingDamage") && meleeData["basePiercingDamage"].IsNumber())
			pMelee->basePiercingDamage = meleeData["basePiercingDamage"].GetFloat();

		if (meleeData.HasMember("baseBluntDamage") && meleeData["baseBluntDamage"].IsNumber())
			pMelee->baseBluntDamage = meleeData["baseBluntDamage"].GetFloat();

		outDef.traits.push_back(std::move(pMelee));
	}

	void ParseItemTraits(const rapidjson::Document& doc, SItemDefinition& outDef)
	{
		const auto& traitsObj = doc["traits"];

		if (traitsObj.HasMember("condition") && traitsObj["condition"].IsObject())
			ParseConditionTrait(traitsObj, outDef);

		if (traitsObj.HasMember("melee_combat") && traitsObj["melee_combat"].IsObject())
			ParseMeleeCombatTrait(traitsObj, outDef);
	}
}

bool CItemParser::Parse(std::vector<char>& rawBuffer, SItemDefinition& outDef)
{
	if (rawBuffer.empty()) return false;

	rapidjson::Document doc;
	doc.ParseInsitu(rawBuffer.data());
	if (doc.HasParseError() || !doc.IsObject()) return false;

	if (doc.HasMember("ItemID") && doc["ItemID"].IsString())
		outDef.id = StringUtils::GenerateID(doc["ItemID"].GetString());
	else
		return false;

	ParseItemTraits(doc, outDef);

	return true;
}