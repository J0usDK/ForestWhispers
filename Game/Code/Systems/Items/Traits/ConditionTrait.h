#pragma once
#include "Base/IItemTrait.h"


class TConditionTrait final : public IItemTrait
{
public:
	static constexpr TTraitID ID = MAKE_TRAIT_ID("condition");

	TTraitID GetTraitID() const override { return ID; }

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

	bool IsEqual(const IItemTrait* pOther) const override
	{
		const auto* pOtherCond = static_cast<const TConditionTrait*>(pOther);
		return maxDurability == pOtherCond->maxDurability && currentDurability == pOtherCond->currentDurability;
	}
};