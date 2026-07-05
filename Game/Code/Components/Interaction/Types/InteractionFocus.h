#pragma once
#include <CryEntitySystem/IEntity.h>
#include "InteractionTypes.h"

struct SInteractionFocus
{
	EntityId entityID = INVALID_ENTITYID;
	EInteractionType type = EInteractionType::None;
	uint32 interactionStringKey = 0u;

	bool IsValid() const { return entityID != INVALID_ENTITYID && type != EInteractionType::None; }
	void Reset() { entityID = INVALID_ENTITYID; type = EInteractionType::None; }

	bool operator==(const SInteractionFocus& other) const { return entityID == other.entityID && type == other.type; }
	bool operator!=(const SInteractionFocus& other) const { return !(*this == other); }

	bool IsEqual(EntityId entityID, EInteractionType type) const { return this->entityID == entityID && this->type == type; }
};