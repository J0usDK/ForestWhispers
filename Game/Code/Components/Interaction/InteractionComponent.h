#pragma once

#include <CryEntitySystem/IEntitySystem.h>

#include "InteractionTypes.h"

struct SInteractionFocus
{
	EntityId entityID = INVALID_ENTITYID;
	EInteractionType type = EInteractionType::None;

	bool IsValid() const { return entityID != INVALID_ENTITYID && type != EInteractionType::None; }
	void Reset() { entityID = INVALID_ENTITYID; type = EInteractionType::None; }
};

enum class ERaycastSource
{
	Camera = 0,
	EntityTransform = 1
};

static void ReflectType(Schematyc::CTypeDesc<ERaycastSource>& desc)
{
	desc.SetGUID("{5303E65E-2BEB-4C78-9A21-7E0C37C42B05}"_cry_guid);
	desc.SetLabel("Raycast Source");
	
	desc.AddConstant(ERaycastSource::Camera, "Camera", "Camera (Player)");
	desc.AddConstant(ERaycastSource::EntityTransform, "EntityTransform", "Entity Transform (NPC/Objects)");
}

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

		desc.AddMember(&CInteractionComponent::m_raySource, 'src', "RaySource", "Raycast Source", "Where the ray originates from", ERaycastSource::Camera);
		desc.AddMember(&CInteractionComponent::m_interactionRange, 'rng', "Range", "Interaction Range", "Max distance in meters to interact with items", 0.0f);
	}

	const SInteractionFocus& GetFocus() const;

private:
	void RefreshFocus();
	bool GetRayParams(Vec3& outPos, Vec3& outDir) const;
	IEntity* PerformRaycast(const Vec3& pos, const Vec3& dir) const;
	bool TryResolveInteraction(const IEntity* pHitEntity, EntityId& outID, EInteractionType& outType) const;
	void UpdateFocus(const EntityId entityID, const EInteractionType type);

private:
	float m_interactionRange = 0.0f;
	ERaycastSource m_raySource = ERaycastSource::Camera;

	SInteractionFocus m_focus;
};