#pragma once
#include "Base/IItemTrait.h"

constexpr TTraitID TRAIT_CONDITION = MAKE_TRAIT_ID("condition");

class TConditionTrait final : public IItemTrait
{
public:
	TTraitID GetTraitID() const override { return TRAIT_CONDITION; }

	float maxDurability = 0.0f;
	float currentDurability = 0.0f;

	float GetHealthPercentage() const
	{
		return (maxDurability > 0.0f) ? (currentDurability / maxDurability) : 0.0f;
	}

	std::unique_ptr<IItemTrait> Clone() const override
	{
		return std::make_unique<TConditionTrait>(*this);
	}
};