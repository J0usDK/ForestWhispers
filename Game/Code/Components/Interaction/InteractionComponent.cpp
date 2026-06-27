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
			PerformRaycast();
			break;
		case Cry::Entity::EEvent::Reset:
			m_focusedEntityID = INVALID_ENTITYID;
			break;
	}
}

void CInteractionComponent::PerformRaycast()
{
	const CCamera& camera = gEnv->pSystem->GetViewCamera();

	Vec3 pos = camera.GetPosition();
	Vec3 dir = camera.GetViewdir() * m_interactionRange;

	ray_hit hit;

	int objTypes = ent_static | ent_rigid | ent_sleeping_rigid;
	unsigned int flags = rwi_stop_at_pierceable;

	int hits = gEnv->pPhysicalWorld->RayWorldIntersection(pos, dir, objTypes, flags, &hit, 1);

	if (hits == 0 || !hit.pCollider)
	{
		m_focusedEntityID = INVALID_ENTITYID;
		return;
	}

	IEntity* pEntity = gEnv->pEntitySystem->GetEntityFromPhysics(hit.pCollider);
	if (!pEntity)
	{
		m_focusedEntityID = INVALID_ENTITYID;
		return;
	}

	if (pEntity->GetComponent<CInteractableComponent>())
		m_focusedEntityID = pEntity->GetId();
	else
		m_focusedEntityID = INVALID_ENTITYID;

}

EntityId CInteractionComponent::GetFocusedEntityID() const
{
	return m_focusedEntityID;
}