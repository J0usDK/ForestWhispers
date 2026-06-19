#pragma once
#include <string>
#include <vector>
#include "Systems/Items/Traits/Base/IItemTrait.h"

using TItemDefinitionID = uint64_t;

struct SItemDefinition
{
	TItemDefinitionID id = 0;

	string name;
	string description;

	string geometryPath;
	string iconPath;

	std::vector<std::unique_ptr<IItemTrait>> traits;

	SItemDefinition() = default;
	SItemDefinition(const SItemDefinition&) = delete;
	SItemDefinition& operator=(const SItemDefinition&) = delete;

	SItemDefinition(SItemDefinition&& other) noexcept :
		id(std::move(other.id)),
		name(std::move(other.name)),
		description(std::move(other.description)),
		geometryPath(std::move(other.geometryPath)),
		iconPath(std::move(other.iconPath)),
		traits(std::move(other.traits))
	{ }

	SItemDefinition& operator=(SItemDefinition&& other) noexcept
	{
		if (this != &other)
		{
			id = std::move(other.id);
			name = std::move(other.name);
			description = std::move(other.description);
			geometryPath = std::move(other.geometryPath);
			iconPath = std::move(other.iconPath);
			traits = std::move(other.traits);
		}
		return *this;
	}
};