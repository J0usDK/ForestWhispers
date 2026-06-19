#include "StdAfx.h"

#include "Global/Utils/JsonUtils.h"
#include "Systems/Items/Parser/TraitParsers.h"
#include "Systems/Items/Traits/MeleeCombatTrait.h"

namespace TraitParsers
{
	void ParseMeleeCombat(const rapidjson::Value& data, SItemDefinition& outDef)
	{
		auto pMelee = std::make_unique<TMeleeCombatTrait>();

		JsonUtils::TryGetFloat(data, "baseSlashingDamage", pMelee->baseSlashingDamage);
		JsonUtils::TryGetFloat(data, "basePiercingDamage", pMelee->basePiercingDamage);
		JsonUtils::TryGetFloat(data, "baseBluntDamage", pMelee->baseBluntDamage);

		outDef.traits.push_back(std::move(pMelee));
	}
}