#pragma once

#include <CryEntitySystem/IEntitySystem.h>

class CInteractionComponent final : public IEntityComponent
{
public:
	CInteractionComponent() = default;
	virtual ~CInteractionComponent() = default;

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	static void ReflectType(Schematyc::CTypeDesc<CInteractionComponent>& desc)
	{
		desc.SetGUID("{6651BEB0-888E-44E1-A840-796635533855}"_cry_guid);

		desc.SetEditorCategory("RPG Systems");
		desc.SetLabel("Interactor");
		desc.SetDescription("Casts a ray to detect interactable items in the world");

		desc.AddMember(&CInteractionComponent::m_interactionRange, 'rng', "Range", "Interaction Range", "Max distance in meters to interact with items", 0.0f);
	}

	EntityId GetFocusedEntityID() const;

private:
	void PerformRaycast();

private:
	float m_interactionRange;
	EntityId m_focusedEntityID;
};