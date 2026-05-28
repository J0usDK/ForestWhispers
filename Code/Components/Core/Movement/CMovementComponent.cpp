#include "StdAfx.h"
#include "CMovementComponent.h"
#include "Components/Core/Global/Utils/Math.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>

namespace
{
	static void RegisterMovementComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CMovementComponent));
		}
	}
	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterMovementComponent);
}

void CMovementComponent::Initialize()
{
	m_pCharacterController = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
}

Cry::Entity::EventFlags CMovementComponent::GetEventMask() const
{
	Cry::Entity::EventFlags flags = Cry::Entity::EventFlags();

	if (m_bIsUpdating)
		flags |= Cry::Entity::EEvent::Update;

	return flags;
}

void CMovementComponent::SetUpdateActive(bool bActive)
{
	if (m_bIsUpdating == bActive)
		return;
	
	m_bIsUpdating = bActive;
	m_pEntity->UpdateComponentEventMask(this);
}

void CMovementComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case Cry::Entity::EEvent::Update:
		{
			float frameTime = gEnv->pTimer->GetFrameTime();

			Move(frameTime);
			Rotate(frameTime);

			if (!m_bIsMoving && !m_bIsRotating)
				SetUpdateActive(false);
			break;
		}
	}
}

void CMovementComponent::Move(float frameTime)
{
	Vec3 targetVelocity = m_movementParams.targetDirection * m_movementParams.maxSpeed;

	float dotProduct = m_currentVelocity.Dot(targetVelocity);
	float accelRate = (dotProduct < 0.0f || targetVelocity.GetLengthSquared() < m_currentVelocity.GetLengthSquared()
		? m_movementParams.deceleration
		: m_movementParams.acceleration);
	float t = std::min(accelRate * frameTime, 1.0f);
	m_currentVelocity = Vec3::CreateLerp(m_currentVelocity, targetVelocity, t);

	if (!Math::IsNearlyZero(m_currentVelocity.GetLengthSquared(), Math::DEFAULT_SQR_EPSILON))
	{
		m_pCharacterController->SetVelocity(m_currentVelocity);
		m_bIsMoving = true;
	}
	else if (m_bIsMoving)
	{
		Stop();
		m_bIsMoving = false;
	}
}

void CMovementComponent::Stop()
{
	m_pCharacterController->SetVelocity(ZERO);
	m_currentVelocity = ZERO;
}

void CMovementComponent::Rotate(float frameTime)
{
	if (Math::IsNearlyZero(m_rotationParams.turnSpeed))
	{
		m_bIsRotating = false;
		return;
	}

	Quat currentRot = m_pEntity->GetRotation();

	if (Quat::IsEquivalent(currentRot, m_rotationParams.targetRotation, Math::DEFAULT_EPSILON))
	{
		m_bIsRotating = false;
		return;
	}

	m_bIsRotating = true;

	float t = std::min(m_rotationParams.turnSpeed * frameTime, 1.0f);
	Quat newRot = Quat::CreateNlerp(currentRot, m_rotationParams.targetRotation, t);
	m_pEntity->SetRotation(newRot);
}

void CMovementComponent::SetMovementRequest(const SMovementParams& params)
{
	CRY_MATH_ASSERT(params.targetDirection.IsValid() && (params.targetDirection.IsZero() || params.targetDirection.IsUnit()));
	m_movementParams = params;

	if (!Math::IsNearlyZero(m_movementParams.targetDirection.GetLengthSquared(), Math::DEFAULT_SQR_EPSILON)
		|| !Math::IsNearlyZero(m_currentVelocity.GetLengthSquared(), Math::DEFAULT_SQR_EPSILON))
	{
		SetUpdateActive(true);
	}
}

void CMovementComponent::SetRotationRequest(const SRotationParams& params)
{
	CRY_MATH_ASSERT(params.targetRotation.IsValid() && (params.targetRotation.IsZero() || params.targetRotation.IsUnit()));
	m_rotationParams = params;

	if (!Math::IsNearlyZero(m_rotationParams.turnSpeed)
		&& !Quat::IsEquivalent(m_pEntity->GetRotation(), m_rotationParams.targetRotation, Math::DEFAULT_EPSILON))
	{
		SetUpdateActive(true);
	}
}

float CMovementComponent::GetCurrentSpeed() const { return m_currentVelocity.GetLength(); }
float CMovementComponent::GetCurrentSpeedSquared() const { return m_currentVelocity.GetLengthSquared(); }
Vec3 CMovementComponent::GetCurrentVelocity() const { return m_currentVelocity; }