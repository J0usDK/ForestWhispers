#pragma once
#include <string>
#include <vector>
#include "Systems/Items/Traits/Base/IItemTrait.h"

using TItemDefinitionID = uint64_t;

struct SItemDefinition
{
	TItemDefinitionID id = 0;
	std::string name;
	std::vector<std::unique_ptr<IItemTrait>> traits;
};