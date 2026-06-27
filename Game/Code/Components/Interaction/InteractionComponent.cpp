#include "StdAfx.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryPhysics/physinterface.h>

#include "InteractionComponent.h"
#include "InteractableComponent.h"

namespace
{
	static void RegisterInteractionComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CInteractionComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterInteractionComponent);
}

void CInteractionComponent::Initialize()
{

}

Cry::Entity::EventFlags CInteractionComponent::GetEventMask() const
{
	return Cry::Entity::EEvent::Update | Cry::Entity::EEvent::Reset;
}

void CInteractionComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case Cry::Entity::EEvent::Update:
			RefreshFocus();
			break;
		case Cry::Entity::EEvent::Reset:
			m_focus.Reset();
			break;
	}
}

void CInteractionComponent::RefreshFocus()
{
	Vec3 pos, dir;

	if (!GetRayParams(pos, dir))
	{
		m_focus.Reset();
		return;
	}

	EntityId entityID;
	EInteractionType type;

	IEntity* pHitEntity = PerformRaycast(pos, dir);
	if (TryResolveInteraction(pHitEntity, entityID, type))
		UpdateFocus(entityID, type);
	else
		m_focus.Reset();
}

bool CInteractionComponent::GetRayParams(Vec3& outPos, Vec3& outDir) const
{
	switch (m_raySource)
	{
		case ERaycastSource::Camera:
		{
			const CCamera& camera = gEnv->pSystem->GetViewCamera();
			outPos = camera.GetPosition();
			outDir = camera.GetViewdir() * m_interactionRange;
			return true;
		}
		case ERaycastSource::EntityTransform:
		{
			outPos = m_pEntity->GetWorldPos();
			outDir = m_pEntity->GetForwardDir() * m_interactionRange;
			return true;
		}
		default:
			return false;
	}
}

IEntity* CInteractionComponent::PerformRaycast(const Vec3& pos, const Vec3& dir) const
{
	ray_hit hit;

	int objTypes = ent_static | ent_rigid | ent_sleeping_rigid;
	unsigned int flags = rwi_stop_at_pierceable;

	int hits = gEnv->pPhysicalWorld->RayWorldIntersection(pos, dir, objTypes, flags, &hit, 1);

	if (hits > 0 && hit.pCollider)
		return gEnv->pEntitySystem->GetEntityFromPhysics(hit.pCollider);
	return nullptr;
}

bool CInteractionComponent::TryResolveInteraction(const IEntity* pHitEntity, EntityId& outID, EInteractionType& outType) const
{
	if (!pHitEntity)
		return false;

	if (auto* pInteractable = pHitEntity->GetComponent<CInteractableComponent>())
	{
		outID = pHitEntity->GetId();
		outType = pInteractable->GetInteractionType();
		return true;
	}
	return false;
}

void CInteractionComponent::UpdateFocus(const EntityId entityID, const EInteractionType type)
{
	m_focus.entityID = entityID;
	m_focus.type = type;
}

const SInteractionFocus& CInteractionComponent::GetFocus() const
{
	return m_focus;
}