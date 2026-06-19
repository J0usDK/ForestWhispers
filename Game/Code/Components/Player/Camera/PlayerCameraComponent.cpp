#include "StdAfx.h"
#include "PlayerCameraComponent.h"
#include "Global/Utils/Math.h"

#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"

namespace
{
	static void RegisterPlayerCameraComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerCameraComponent));
		}
	}
	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerCameraComponent);
}

void CPlayerCameraComponent::Initialize()
{
	m_pCameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();

	m_cameraState.localYaw = CCamera::CreateAnglesYPR(Matrix33(m_pEntity->GetWorldRotation())).x;
	m_cameraState.localPitch = 0.0f;

	UpdateCameraCache();
}

Cry::Entity::EventFlags CPlayerCameraComponent::GetEventMask() const
{
	return Cry::Entity::EEvent::Update;
}

void CPlayerCameraComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case Cry::Entity::EEvent::Update:
			UpdateCameraTransform();
			break;
	}
}

void CPlayerCameraComponent::ApplyInputDelta(const Vec2& lookDelta)
{
	if (lookDelta.IsZero())
		return;

	m_cameraState.localYaw += lookDelta.x * m_rotationSpeed;
	m_cameraState.localPitch += lookDelta.y * m_rotationSpeed;

	m_cameraState.localPitch = CLAMP(m_cameraState.localPitch, m_pitchMin, m_pitchMax);
	m_cameraState.localYaw = std::remainderf(m_cameraState.localYaw, 2.0f * g_PI);

	UpdateCameraCache();
}

void CPlayerCameraComponent::UpdateCameraCache()
{
	m_cameraCache.lookRotation = m_cameraState.baseFrameRotation 
		* Quat::CreateRotationZ(m_cameraState.localYaw)
		* Quat::CreateRotationX(m_cameraState.localPitch);

	m_cameraCache.absoluteYaw = m_cameraState.baseYaw + m_cameraState.localYaw;
	m_cameraCache.absolutePitch = m_cameraState.basePitch + m_cameraState.localPitch;
}

void CPlayerCameraComponent::UpdateCameraTransform()
{
	if (!m_pCameraComponent) return;

	Vec3 worldPosition = m_cameraState.pivotPosition + (m_pEntity->GetWorldRotation() * m_cameraOffset);

	Matrix34 worldMatrix;
	worldMatrix.SetRotation33(Matrix33(m_cameraCache.lookRotation));
	worldMatrix.SetTranslation(worldPosition);

	Matrix34 localMatrix = m_pEntity->GetWorldTM().GetInvertedFast() * worldMatrix;
	m_pCameraComponent->SetTransformMatrix(localMatrix);

}

void CPlayerCameraComponent::SetPivotPosition(const Vec3& worldPosition)
{
	m_cameraState.pivotPosition = worldPosition;
}

void CPlayerCameraComponent::SetBaseFrameRotation(const Quat& worldRotation)
{
	m_cameraState.baseFrameRotation = worldRotation;

	Ang3 baseAngles = CCamera::CreateAnglesYPR(Matrix33(worldRotation));
	m_cameraState.baseYaw = baseAngles.x;
	m_cameraState.basePitch = baseAngles.y;

	UpdateCameraCache();
}

float CPlayerCameraComponent::GetAbsoluteYaw() const
{
	return m_cameraCache.absoluteYaw;
}

float CPlayerCameraComponent::GetAbsolutePitch() const
{
	return m_cameraCache.absolutePitch;
}

Quat CPlayerCameraComponent::GetLookOrientation() const
{
	return m_cameraCache.lookRotation;
}