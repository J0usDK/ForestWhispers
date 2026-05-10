// Copyright 2017-2020 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

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
		desc.AddMember(&CPlayerComponent::m_movementSpeed, 'pms', "playermovementspeed", "Player Movement Speed", "The speed at which the player moves", 0.0f);
		desc.AddMember(&CPlayerComponent::m_rotationSpeed, 'prs', "playerrotationspeed", "Player Rotation Speed", "The speed at which the player rotates", 0.0f);
		desc.AddMember(&CPlayerComponent::m_defaultCameraPos, 'dcp', "defaultcamerapos", "Default Camera Position", "The default position of the camera", Vec3(0.0f, 0.0f, 0.0f));
		desc.AddMember(&CPlayerComponent::m_rotationLimitsMinPitch, 'cpmi', "rotationlimitsminpitch", "Rotation Limits Min Pitch", "Minimum pitch rotation limit for the player camera", 0.0f);
		desc.AddMember(&CPlayerComponent::m_rotationLimitsMaxPitch, 'cpma', "rotationlimitsmaxpitch", "Rotation Limits Max Pitch", "Maximum pitch rotation limit for the player camera", 0.0f);
	}

private:
	//Camera Component
	Cry::DefaultComponents::CCameraComponent* m_pCameraComponent = nullptr;
	Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;
	Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterController = nullptr;
	Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAdvancedAnimationComponent = nullptr;

	void InitializeInput();
	void PlayerMovement();
	void PlayerRotation();

	Quat m_lookOrientation;
	Vec3 m_defaultCameraPos;

	Vec2 m_movementDelta;
	Vec2 m_mouseDeltaRotation;

	float m_movementSpeed;
	float m_rotationSpeed;
	float m_rotationLimitsMinPitch;
	float m_rotationLimitsMaxPitch;
};
