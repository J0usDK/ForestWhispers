// Copyright 2017-2020 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

#include "Components/Core/Movement/MovementComponent.h"
#include "Components/Core/Animation/AnimationComponent.h"
#include "Components/Player/Camera/PlayerCameraComponent.h"
#include "Components/Player/Input/PlayerInputComponent.h"
#include "Components/Interaction/InteractionComponent.h"

////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////
class CPlayerComponent final : public IEntityComponent
{	
public:
	CPlayerComponent() = default;
	virtual ~CPlayerComponent() = default;

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
	{
		desc.SetGUID("{63F4C0C6-32AF-4ACB-8FB0-57D45DD14725}"_cry_guid);
		
		desc.AddMember(&CPlayerComponent::m_defaultCameraPivot, 'dcp', "defaultcamerapivot", "Default Camera Pivot", "Default camera pivot coordinates", ZERO);
		desc.AddMember(&CPlayerComponent::m_maxWalkSpeed, 'mws', "maxwalkspeed", "Max Walk Speed", "Maximum walking speed", 0.0f);
		desc.AddMember(&CPlayerComponent::m_maxRunSpeed, 'mrs', "maxrunspeed", "Max Run Speed", "Maximum running speed", 0.0f);
		desc.AddMember(&CPlayerComponent::m_acceleration, 'acc', "acceleration", "Acceleration", "How fast the player reaches max speed", 0.0f);
		desc.AddMember(&CPlayerComponent::m_deceleration, 'dec', "deceleration", "Deceleration", "How fast the player stops", 0.0f);
		desc.AddMember(&CPlayerComponent::m_turnSpeed, 'ts', "turnspeed", "Turn Speed", "How fast the player's body rotates", 0.0f);
	}

private:
	void UpdateCamera(const SCharacterIntent& intent);
	void UpdateMovement(const SCharacterIntent& intent);
	void UpdateAnimation();
	void UpdateInteraction(const SCharacterIntent& intent);

private:
	CMovementComponent* m_pMovement = nullptr;
	CAnimationComponent* m_pAnimator = nullptr;
	CPlayerCameraComponent* m_pCamera = nullptr;
	CPlayerInputComponent* m_pInput = nullptr;
	CInteractionComponent* m_pInteractor = nullptr;

	Vec3 m_defaultCameraPivot = ZERO;

	float m_maxWalkSpeed = 0.0f;
	float m_maxRunSpeed = 0.0f;
	float m_acceleration = 0.0f;
	float m_deceleration = 0.0f;
	float m_turnSpeed = 0.0f;

	bool m_bIsPlaying = false;
	int16 m_headBoneID = -1;

	enum class EPlayerState
	{
		Idle,
		Walk,
		Run
	} m_currentState;

	const char* m_kHeadBoneName = "CC_Base_Head";
	const char* m_kIdleName = "Idle";
	const char* m_kWalkName = "Walk";
	const char* m_kRunName = "Run";
};
