#pragma once

#include "Global/Wrappers/RapidJsonWrapper.h"
#include "Systems/Items/Database/ItemDefinition.h"

using TTraitParserFunc = void(*)(const rapidjson::Value&, SItemDefinition&);

namespace TraitParsers
{
	void ParseCondition(const rapidjson::Value& data, SItemDefinition& outDef);
	void ParseMeleeCombat(const rapidjson::Value& data, SItemDefinition& outDef);
}