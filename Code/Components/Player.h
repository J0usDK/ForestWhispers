// Copyright 2017-2020 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once
#include <optional>

#include <DefaultComponents/Cameras/CameraComponent.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Geometry/AdvancedAnimationComponent.h>

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
		desc.AddMember(&CPlayerComponent::m_rotationSpeed, 'prs', "playerrotationspeed", "Player Rotation Speed", "The speed at which the player rotates", 0.0f);
		desc.AddMember(&CPlayerComponent::m_cameraOffset, 'cof', "cameraoffset", "Camera Offset", "The offset of the camera position", Vec3(0.0f, 0.0f, 0.0f));
		desc.AddMember(&CPlayerComponent::m_rotationLimitsMinPitch, 'cpmi', "rotationlimitsminpitch", "Rotation Limits Min Pitch", "Minimum pitch rotation limit for the player camera", 0.0f);
		desc.AddMember(&CPlayerComponent::m_rotationLimitsMaxPitch, 'cpma', "rotationlimitsmaxpitch", "Rotation Limits Max Pitch", "Maximum pitch rotation limit for the player camera", 0.0f);

		desc.AddMember(&CPlayerComponent::m_maxWalkSpeed, 'mws', "maxwalkspeed", "Max Walk Speed", "Maximum walking speed", 0.0f);
		desc.AddMember(&CPlayerComponent::m_acceleration, 'acc', "acceleration", "Acceleration", "How fast the player reaches max speed", 0.0f);
		desc.AddMember(&CPlayerComponent::m_deceleration, 'dec', "deceleration", "Deceleration", "How fast the player stops", 0.0f);

		desc.AddMember(&CPlayerComponent::m_maxRunSpeed, 'mrs', "maxrunspeed", "Max Run Speed", "Maximum running speed", 0.0f);
	}

private:
	// States
	bool m_bIsPlaying = false;
	enum class EPlayerState
	{
		Idle,
		Walking,
		Running
	};

	// Components
	Cry::DefaultComponents::CCameraComponent* m_pCameraComponent = nullptr;
	Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;
	Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterController = nullptr;
	Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAdvancedAnimationComponent = nullptr;

	// Methods
	void InitializeInput();
	void PlayerMovement();
	void PlayerRotation();
	void UpdateAnimationState(float currentSpeed);
	void SetPlayerState(EPlayerState state);

	std::optional<Matrix34> GetHeadBoneWorldMatrix();
	Matrix34 GetBoneWorldMatrix(ISkeletonPose* skelPose, int16 boneID);

	// Movement Data
	EPlayerState m_currentState;
	Vec2 m_movementDelta = ZERO;

	Vec2 m_currentVelocity = ZERO;
	float m_currentTravelAngle = 0.0f;
	float m_acceleration = 5.0f;
	float m_deceleration = 8.0f;
	float m_maxWalkSpeed = 1.5f;

	bool m_bWantsToSprint = false;
	float m_maxRunSpeed = 0.0f;

	// Camera Data
	Quat m_lookOrientation = IDENTITY;
	Vec3 m_cameraOffset = ZERO;
	Vec2 m_mouseDeltaRotation = ZERO;

	float m_rotationSpeed;
	float m_rotationLimitsMinPitch;
	float m_rotationLimitsMaxPitch;

	// Cached Data
	Vec3 m_lastMoveDirection = Vec3(0.0f, 1.0f, 0.0f);
	int16 m_headBoneID = -1;
};
