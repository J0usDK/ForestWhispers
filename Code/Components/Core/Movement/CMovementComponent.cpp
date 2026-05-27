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
	return Cry::Entity::EEvent::Update;
}

void CMovementComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case Cry::Entity::EEvent::Update:
			float frameTime = gEnv->pTimer->GetFrameTime();
			Move(frameTime);
			Rotate(frameTime);
			break;
	}
}

void CMovementComponent::Move(float frameTime)
{
	Vec3 targetVelocity = m_movementParams.targetDirection * m_movementParams.maxSpeed;
	float accelRate = (targetVelocity.GetLengthSquared() > m_currentVelocity.GetLengthSquared()) ? m_movementParams.acceleration : m_movementParams.deceleration;
	m_currentVelocity = Vec3::CreateLerp(m_currentVelocity, targetVelocity, accelRate * frameTime);

	if (Math::IsNearlyZero(m_currentVelocity.GetLength()))
		m_pCharacterController->SetVelocity(m_currentVelocity);
	else
		Stop();
}

void CMovementComponent::Stop()
{
	m_pCharacterController->SetVelocity(ZERO);
	m_currentVelocity = ZERO;
}

void CMovementComponent::Rotate(float frameTime)
{
	if (Math::IsNearlyZero(m_rotationParams.turnSpeed))
		return;

	Quat currentRot = m_pEntity->GetRotation();
	Quat newRot = Quat::CreateSlerp(currentRot, m_rotationParams.targetRotation, m_rotationParams.turnSpeed * frameTime);
	newRot.NormalizeSafe();

	m_pEntity->SetRotation(newRot);
}

void CMovementComponent::SetMovementRequest(const SMovementParams& params)
{
	m_movementParams = params;
	m_movementParams.targetDirection.NormalizeSafe();
}

void CMovementComponent::SetRotationRequest(const SRotationParams& params)
{
	m_rotationParams = params;
	m_rotationParams.targetRotation.NormalizeSafe();
}

float CMovementComponent::GetCurrentSpeed() const { return m_currentVelocity.GetLength(); }
Vec3 CMovementComponent::GetCurrentVelocity() const { return m_currentVelocity; }