#pragma once
#include "Global/Utils/StringUtils.h"
#include <cstdint>

#define MAKE_TRAIT_ID(trait_name) StringUtils::GenerateID(#trait_name)

using TTraitID = uint32_t;

class IItemTrait
{
public:
	virtual ~IItemTrait() = default;
	virtual TTraitID GetTraitID() const = 0;
};