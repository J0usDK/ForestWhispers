#pragma once

#include <CryEntitySystem/IEntity.h>
#include "Components/Interaction/InteractionTypes.h"

struct SInteractionContext
{
	IEntity* actor;
	IEntity* target;

	bool IsValid() const { return actor && target; }
};