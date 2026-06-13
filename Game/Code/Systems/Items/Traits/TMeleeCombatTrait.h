#pragma once
#include "Base/IItemTrait.h"

constexpr TTraitID TRAIT_MELEE_COMBAT = MAKE_TRAIT_ID("melee_combat");

class TMeleeCombatTrait final : public IItemTrait
{
public:
	TTraitID GetTraitID() const override { return TRAIT_MELEE_COMBAT; }

	std::unique_ptr<IItemTrait> Clone() const override
	{
		return std::make_unique<TMeleeCombatTrait>(*this);
	}

	float baseSlashingDamage = 0.0f;
	float basePiercingDamage = 0.0f;
	float baseBluntDamage = 0.0f;
};