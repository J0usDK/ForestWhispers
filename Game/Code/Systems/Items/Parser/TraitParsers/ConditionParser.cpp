#include "StdAfx.h"

#include "Global/Utils/JsonUtils.h"
#include "Systems/Items/Parser/TraitParsers.h"
#include "Systems/Items/Traits/ConditionTrait.h"

namespace TraitParsers
{
	void ParseCondition(const rapidjson::Value& data, SItemDefinition& outDef)
	{
		auto pCondition = std::make_unique<TConditionTrait>();

		JsonUtils::TryGetFloat(data, "maxDurability", pCondition->maxDurability);
		JsonUtils::TryGetFloat(data, "currentDurability", pCondition->currentDurability);

		outDef.traits.push_back(std::move(pCondition));
	}
}