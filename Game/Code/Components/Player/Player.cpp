// Copyright 2017-2020 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "Player.h"

#include "Global/Utils/Math.h"
#include "Global/GameEnv.h"
#include "Services/Base/InteractionService.h"
#include "Systems/UI/UISystem.h"
#include "Systems/UI/UIStringTable.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryAnimation/ICryAnimation.h>

namespace
{
	static void RegisterPlayerComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent);
}

void CPlayerComponent::Initialize()
{
	m_pMovement = m_pEntity->GetOrCreateComponent<CMovementComponent>();
	m_pAnimator = m_pEntity->GetOrCreateComponent<CAnimationComponent>();
	m_pCamera = m_pEntity->GetOrCreateComponent<CPlayerCameraComponent>();
	m_pInput = m_pEntity->GetOrCreateComponent<CPlayerInputComponent>();
	m_pInteractor = m_pEntity->GetOrCreateComponent<CInteractionComponent>();

	m_pInteractor->AddListener(this);
}

Cry::Entity::EventFlags CPlayerComponent::GetEventMask() const
{
	return Cry::Entity::EEvent::GameplayStarted | Cry::Entity::EEvent::Update | Cry::Entity::EEvent::Reset;
}

void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case Cry::Entity::EEvent::GameplayStarted:
		{
			m_bIsPlaying = true;
			break;
		}
		case Cry::Entity::EEvent::Update:
		{
			if (!m_bIsPlaying || !m_pMovement || !m_pAnimator || !m_pCamera || !m_pInput)
				break;

			const SCharacterIntent& intent = m_pInput->GetCurrentIntent();
			UpdateCamera(intent);
			UpdateMovement(intent);
			UpdateInteraction(intent);
			UpdateAnimation();
			break;
		}
		case Cry::Entity::EEvent::Reset:
		{
			m_bIsPlaying = false;
			m_headBoneID = -1;
			m_currentState = EPlayerState::Idle;
			if (m_pAnimator)
				m_pAnimator->QueueFragment(m_kIdleName);
			break;
		}
	}
}

void CPlayerComponent::UpdateCamera(const SCharacterIntent& intent)
{
	m_pCamera->ApplyInputDelta(intent.lookDelta);

	if (m_headBoneID == -1)
		if (auto boneID = m_pAnimator->GetBoneID(m_kHeadBoneName))
			m_headBoneID = *boneID;

	if (m_headBoneID >= 0)
	{
		if (auto headPos = m_pAnimator->GetBoneWorldPosition(m_headBoneID))
			m_pCamera->SetPivotPosition(*headPos);
	}
	else
		m_pCamera->SetPivotPosition(m_pEntity->GetWorldPos() + m_defaultCameraPivot);
}

void CPlayerComponent::UpdateMovement(const SCharacterIntent& intent)
{
	SMovementParams moveParams;
	SRotationParams rotParams;

	Quat camYawRot = Quat::CreateRotationZ(m_pCamera->GetAbsoluteYaw());

	if (!Math::IsNearlyZero(intent.movement.GetLengthSquared()))
	{
		bool bCanSprint = intent.sprint && (intent.movement.y > Math::DEFAULT_EPSILON);
		moveParams.maxSpeed = bCanSprint ? m_maxRunSpeed : m_maxWalkSpeed;
		
		Vec3 localInput(intent.movement.x, intent.movement.y, 0.0f);
		localInput.NormalizeSafe();
		moveParams.targetDirection = camYawRot * localInput;
	}

	moveParams.acceleration = m_acceleration;
	moveParams.deceleration = m_deceleration;

	rotParams.targetRotation = camYawRot;
	rotParams.turnSpeed = m_turnSpeed;

	m_pMovement->SetMovementRequest(moveParams);
	m_pMovement->SetRotationRequest(rotParams);
}

void CPlayerComponent::UpdateInteraction(const SCharacterIntent& intent)
{
	if (!intent.interact || !m_pInteractor)
		return;

	const SInteractionFocus& focus = m_pInteractor->GetFocus();
	if (!focus.IsValid())
		return;

	gGameEnv->pInteractionService->ExecuteInteraction(m_pEntity, focus);
}

void CPlayerComponent::UpdateAnimation()
{
	float currentSpeed = m_pMovement->GetCurrentSpeed();
	Vec3 currentVelocity = m_pMovement->GetCurrentVelocity();

	float travelAngle = 0.0f;
	if (!Math::IsNearlyZero(currentSpeed))
	{
		float localX = currentVelocity.Dot(m_pEntity->GetRightDir());
		float localY = currentVelocity.Dot(m_pEntity->GetForwardDir());
		travelAngle = atan2(localX, localY);
	}

	m_pAnimator->SetMotionParameter(eMotionParamID_TravelSpeed, currentSpeed);
	m_pAnimator->SetMotionParameter(eMotionParamID_TravelAngle, travelAngle, 5.0f, EParamInterpType::Angle);

	EPlayerState targetState = EPlayerState::Idle;
	if (currentSpeed > m_maxWalkSpeed + Math::DEFAULT_EPSILON)
		targetState = EPlayerState::Run;
	else if (!Math::IsNearlyZero(currentSpeed))
		targetState = EPlayerState::Walk;

	if (m_currentState != targetState)
	{
		m_currentState = targetState;
		switch (m_currentState)
		{
			case EPlayerState::Idle:
				m_pAnimator->QueueFragment(m_kIdleName);
				break;
			case EPlayerState::Walk:
				m_pAnimator->QueueFragment(m_kWalkName);
				break;
			case EPlayerState::Run:
				m_pAnimator->QueueFragment(m_kRunName);
				break;
		}
	}
}

void CPlayerComponent::OnFocusChanged(const SInteractionFocus& newFocus)
{
	SShowHintEvent event;
	if (newFocus.IsValid())
	{
		event.stringKey = newFocus.interactionStringKey;
		event.isVisible = true;
	}
	else event.isVisible = false;

	gGameEnv->pUISystem->HandleEvent(event);
}