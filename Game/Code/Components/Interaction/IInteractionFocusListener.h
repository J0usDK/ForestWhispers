#pragma once
#include "Types/InteractionFocus.h"

class IInteractionFocusListener
{
public:
	virtual ~IInteractionFocusListener() = default;
	virtual void OnFocusChanged(const SInteractionFocus& newFocus) = 0;
};