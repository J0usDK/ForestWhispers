#pragma once
#include "Global/Utils/StringUtils.h"
#include <cstdint>
#include <memory>

#define MAKE_TRAIT_ID(trait_name) StringUtils::GenerateID(#trait_name)

using TTraitID = uint64_t;

class IItemTrait
{
public:
	virtual ~IItemTrait() = default;
	virtual TTraitID GetTraitID() const = 0;

	virtual std::unique_ptr<IItemTrait> Clone() const = 0;
	virtual bool IsEqual(const IItemTrait* pOther) const = 0;
};