#pragma once

#include <CryEntitySystem/IEntityComponent.h>

#include "Components/Interaction/InteractionTypes.h"

class CInteractableComponent final : public IEntityComponent
{
public:
	CInteractableComponent() = default;
	virtual ~CInteractableComponent() = default;

	static void ReflectType(Schematyc::CTypeDesc<CInteractableComponent>& desc)
	{
		desc.SetGUID("{A059ABEA-91D9-4070-BC65-AE8F5A38C3BC}"_cry_guid);

		desc.SetEditorCategory("RPG Systems");
		desc.SetLabel("Interactable Tag");
		desc.SetDescription("Marks entity as interactable and defines its interaction type");

		desc.AddMember(&CInteractableComponent::m_type, 'type', "Type", "Interaction Type", "Defines which service will handle this entity", EInteractionType::None);
	}

	void Setup(EInteractionType type);
	EInteractionType GetInteractionType() const;

private:
	EInteractionType m_type;
};