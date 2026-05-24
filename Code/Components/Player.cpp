// Copyright 2017-2020 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "Player.h"
#include "GamePlugin.h"

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
	m_pCameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();
	m_pCharacterController = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
	m_pAdvancedAnimationComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAdvancedAnimationComponent>();

	m_pAdvancedAnimationComponent->SetCharacterFile("Objects/characters/humans/mc/mc.cdf");
	m_pAdvancedAnimationComponent->SetControllerDefinitionFile("Animations/characters/mc/data/FirstPersonControllerDefinition.xml");
	m_pAdvancedAnimationComponent->SetMannequinAnimationDatabaseFile("Animations/characters/mc/data/firstperson.adb");
	m_pAdvancedAnimationComponent->SetDefaultScopeContextName("FirstPersonCharacter");
	m_pAdvancedAnimationComponent->SetDefaultFragmentName("Idle");
	m_pAdvancedAnimationComponent->SetAnimationDrivenMotion(false);
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
			InitializeInput();
			m_bIsPlaying = true;
			break;
		}
		case Cry::Entity::EEvent::Update:
		{
			if (!m_bIsPlaying)
				break;
			PlayerMovement();
			PlayerRotation();
			break;
		}
		case Cry::Entity::EEvent::Reset:
		{
			m_bIsPlaying = false;

			m_movementDelta = ZERO;
			m_mouseDeltaRotation = ZERO;
			m_lookOrientation = IDENTITY;
			m_headBoneID = -1;

			Matrix34 camDefaultMatrix;
			camDefaultMatrix.SetTranslation(m_cameraOffset);
			camDefaultMatrix.SetRotation33(Matrix33(m_pEntity->GetWorldRotation()));
			m_pCameraComponent->SetTransformMatrix(camDefaultMatrix);
			break;
		}
	}
}

void CPlayerComponent::InitializeInput()
{
	m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) { m_movementDelta.y = value; });
	m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, eKI_W);

	m_pInputComponent->RegisterAction("player", "moveback", [this](int activationMode, float value) { m_movementDelta.y = -value; });
	m_pInputComponent->BindAction("player", "moveback", eAID_KeyboardMouse, eKI_S);

	m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) { m_movementDelta.x = value; });
	m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, eKI_D);

	m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) { m_movementDelta.x = -value; });
	m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, eKI_A);

	m_pInputComponent->RegisterAction("player", "yaw", [this](int activationMode, float value) { m_mouseDeltaRotation.y -= value; });
	m_pInputComponent->BindAction("player", "yaw", eAID_KeyboardMouse, eKI_MouseY);

	m_pInputComponent->RegisterAction("player", "pitch", [this](int activationMode, float value) { m_mouseDeltaRotation.x -= value; });
	m_pInputComponent->BindAction("player", "pitch", eAID_KeyboardMouse, eKI_MouseX);

	m_pInputComponent->RegisterAction("player", "sprint", [this](int activationMode, float value) { m_bWantsToSprint = (value > 0.0f); });
	m_pInputComponent->BindAction("player", "sprint", eAID_KeyboardMouse, eKI_LShift);
}

void CPlayerComponent::PlayerMovement()
{
	float frameTime = gEnv->pTimer->GetFrameTime();
	Vec2 inputDir = m_movementDelta;
	if (inputDir.GetLengthSquared() > 1.0f)
		inputDir.Normalize();

	bool bCanSprint = m_bWantsToSprint && (m_movementDelta.y > 0.1f);
	float targetMaxSpeed = bCanSprint ? m_maxRunSpeed : m_maxWalkSpeed;

	Vec2 targetVelocity = inputDir * targetMaxSpeed;
	float accelRate = (targetVelocity.GetLengthSquared() > m_currentVelocity.GetLengthSquared()) ? m_acceleration : m_deceleration;
	m_currentVelocity = Vec2::CreateLerp(m_currentVelocity, targetVelocity, accelRate * frameTime);

	float currentSpeed = m_currentVelocity.GetLength();
	if (currentSpeed > 0.01f)
		m_lastMoveDirection = Vec3(m_currentVelocity.x, m_currentVelocity.y, 0.0f).GetNormalizedSafe();

	if (currentSpeed > 0.01f)
	{
		Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
		Ang3 flatAngles(ypr.x, 0.0f, 0.0f);
		Quat flatYaw = Quat(CCamera::CreateOrientationYPR(flatAngles));
		Vec3 velocity = flatYaw * m_lastMoveDirection * currentSpeed;
		m_pCharacterController->SetVelocity(velocity);
	}
	else
		m_pCharacterController->SetVelocity(ZERO);
	UpdateAnimationState(currentSpeed);
}
void CPlayerComponent::PlayerRotation()
{
	Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
	ypr.x += m_mouseDeltaRotation.x * m_rotationSpeed;
	ypr.y = CLAMP(ypr.y + m_mouseDeltaRotation.y * m_rotationSpeed, m_rotationLimitsMinPitch, m_rotationLimitsMaxPitch);
	ypr.z = 0.0f;

	m_lookOrientation = Quat(CCamera::CreateOrientationYPR(ypr));
	Ang3 yawOnly(ypr.x, 0.0f, 0.0f);
	m_pEntity->SetRotation(Quat(CCamera::CreateOrientationYPR(yawOnly)));

	Ang3 pitchOnly(0.0f, ypr.y, 0.0f);
	Matrix34 finalCamMatrix;
	finalCamMatrix.SetRotation33(Matrix33(CCamera::CreateOrientationYPR(pitchOnly)));

	Vec3 cameraPosition = m_cameraOffset;
	if (auto boneMat = GetHeadBoneWorldMatrix())
		cameraPosition = boneMat->TransformPoint(m_cameraOffset);

	finalCamMatrix.SetTranslation(cameraPosition);
	m_pCameraComponent->SetTransformMatrix(finalCamMatrix);
	m_mouseDeltaRotation = ZERO;
}

void CPlayerComponent::UpdateAnimationState(float currentSpeed)
{
	if (!m_pAdvancedAnimationComponent) return;

	float angleRad = 0.0f;
	if (currentSpeed > 0.01f)
		angleRad = atan2(m_lastMoveDirection.x, m_lastMoveDirection.y);

	m_pAdvancedAnimationComponent->SetMotionParameter(eMotionParamID_TravelSpeed, currentSpeed);
	m_pAdvancedAnimationComponent->SetMotionParameter(eMotionParamID_TravelAngle, angleRad);

	EPlayerState targetState = EPlayerState::Idle;
	if (currentSpeed > m_maxWalkSpeed + 0.2f)
		targetState = EPlayerState::Running;
	else if (currentSpeed > 0.1f)
		targetState = EPlayerState::Walking;

	if (m_currentState != targetState)
		SetPlayerState(targetState);

}

void CPlayerComponent::SetPlayerState(EPlayerState state)
{
	m_currentState = state;
	switch (m_currentState)
	{
		case EPlayerState::Idle:
			m_pAdvancedAnimationComponent->QueueFragment("Idle");
			break;
		case EPlayerState::Walking:
			m_pAdvancedAnimationComponent->QueueFragment("Walk");
			break;
		case EPlayerState::Running:
			m_pAdvancedAnimationComponent->QueueFragment("Run");
			break;
	}
}

std::optional<Matrix34> CPlayerComponent::GetHeadBoneWorldMatrix()
{
	if (ICharacterInstance* pCharacter = m_pAdvancedAnimationComponent->GetCharacter())
	{
		if (m_headBoneID == -1)
			m_headBoneID = pCharacter->GetIDefaultSkeleton().GetJointIDByName("CC_Base_Head");
		if (m_headBoneID >= 0)
			if (ISkeletonPose* pSkeletonPose = pCharacter->GetISkeletonPose())
				return GetBoneWorldMatrix(pSkeletonPose, m_headBoneID);
	}
	return std::nullopt;
}

Matrix34 CPlayerComponent::GetBoneWorldMatrix(ISkeletonPose* skelPose, int16 boneID)
{
	QuatT boneTransform = skelPose->GetAbsJointByID(boneID);
	return m_pAdvancedAnimationComponent->GetTransformMatrix() * Matrix34(boneTransform);
}