#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/Utils/SharedString.h>

#include "Components/Interaction/Types/InteractionTypes.h"

class CInteractableComponent final : public IEntityComponent
{
public:
	CInteractableComponent() = default;
	virtual ~CInteractableComponent() = default;

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	static void ReflectType(Schematyc::CTypeDesc<CInteractableComponent>& desc)
	{
		desc.SetGUID("{A059ABEA-91D9-4070-BC65-AE8F5A38C3BC}"_cry_guid);

		desc.SetEditorCategory("RPG Systems");
		desc.SetLabel("Interactable Tag");
		desc.SetDescription("Marks entity as interactable and defines its interaction type");

		desc.AddMember(&CInteractableComponent::m_type, 'type', "Type", "Interaction Type", "Defines which service will handle this entity", EInteractionType::None);
		desc.AddMember(&CInteractableComponent::m_editorStringKey, 'skey', "StringKey", "Localization Key", "Key for localization", "");
	}

	void SetInteractionType(EInteractionType type);
	EInteractionType GetInteractionType() const;

	void SetInteractionStringKey(const char* key);
	uint32 GetInteractionStringKey() const;

private:
	EInteractionType m_type;
	Schematyc::CSharedString m_editorStringKey = "";
	uint32 m_interactionStringKey = 0;
};