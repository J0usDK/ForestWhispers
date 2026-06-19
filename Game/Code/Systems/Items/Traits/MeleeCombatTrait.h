#pragma once
#include "Base/IItemTrait.h"


class TMeleeCombatTrait final : public IItemTrait
{
public:
	static constexpr TTraitID ID = MAKE_TRAIT_ID("melee_combat");

	TTraitID GetTraitID() const override { return ID; }

	std::unique_ptr<IItemTrait> Clone() const override
	{
		return std::make_unique<TMeleeCombatTrait>(*this);
	}

	bool IsEqual(const IItemTrait* pOther) const override
	{
		const auto* pOtherMelee = static_cast<const TMeleeCombatTrait*>(pOther);
		return baseSlashingDamage == pOtherMelee->baseSlashingDamage
			&& basePiercingDamage == pOtherMelee->basePiercingDamage
			&& baseBluntDamage == pOtherMelee->baseBluntDamage;
	}

	float baseSlashingDamage = 0.0f;
	float basePiercingDamage = 0.0f;
	float baseBluntDamage = 0.0f;
};