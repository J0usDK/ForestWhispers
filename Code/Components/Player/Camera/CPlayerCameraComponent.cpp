#include "StdAfx.h"
#include "CPlayerCameraComponent.h"

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

	m_cameraState.baseFrameRotation = IDENTITY;
	m_cameraState.localYaw = CCamera::CreateAnglesYPR(Matrix33(m_pEntity->GetWorldRotation())).x;
	m_cameraState.localPitch = 0.0f;
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
	m_cameraState.localYaw += lookDelta.x * m_rotationSpeed;
	m_cameraState.localPitch += lookDelta.y * m_rotationSpeed;

	m_cameraState.localPitch = CLAMP(m_cameraState.localPitch, m_pitchMin, m_pitchMax);

	m_cameraState.localYaw = std::remainderf(m_cameraState.localYaw, 2.0f * g_PI);
}

void CPlayerCameraComponent::UpdateCameraTransform()
{
	if (!m_pCameraComponent) return;

	Quat worldLookRot = GetLookOrientation();
	Vec3 worldPosition = m_cameraState.pivotPosition + (m_pEntity->GetWorldRotation() * m_cameraOffset);

	Matrix34 worldMatrix;
	worldMatrix.SetRotation33(Matrix33(worldLookRot));
	worldMatrix.SetTranslation(worldPosition);

	Matrix34 localMatrix = m_pEntity->GetWorldTM().GetInverted() * worldMatrix;
	m_pCameraComponent->SetTransformMatrix(localMatrix);
}

void CPlayerCameraComponent::SetPivotPosition(const Vec3& worldPosition)
{
	m_cameraState.pivotPosition = worldPosition;
}

void CPlayerCameraComponent::SetBaseFrameRotation(const Quat& worldRotation)
{
	m_cameraState.baseFrameRotation = worldRotation;
}

float CPlayerCameraComponent::GetAbsoluteYaw() const
{
	return CCamera::CreateAnglesYPR(Matrix33(GetLookOrientation())).x;
}

float CPlayerCameraComponent::GetAbsolutePitch() const
{
	return CCamera::CreateAnglesYPR(Matrix33(GetLookOrientation())).y;
}

Quat CPlayerCameraComponent::GetLookOrientation() const
{
	Ang3 localAngles(m_cameraState.localYaw, m_cameraState.localPitch, 0.0f);
	Quat localRot(CCamera::CreateOrientationYPR(localAngles));
	Quat worldRot = m_cameraState.baseFrameRotation * localRot;
	return worldRot.GetNormalized();
}