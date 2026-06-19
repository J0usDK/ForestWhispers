#pragma once

#include <DefaultComponents/Physics/CharacterControllerComponent.h>

struct SMovementParams
{
	Vec3 targetDirection = ZERO;
	float maxSpeed = 0.0f;
	float acceleration = 0.0f;
	float deceleration = 0.0f;
};

struct SRotationParams
{
	Quat targetRotation = IDENTITY;
	float turnSpeed = 0.0f;
};

class CMovementComponent final : public IEntityComponent
{
public:
	CMovementComponent() = default;
	virtual ~CMovementComponent() = default;

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	static void ReflectType(Schematyc::CTypeDesc<CMovementComponent>& desc)
	{
		desc.SetGUID("{EF85A35A-B098-435F-88B1-0604395D9378}"_cry_guid);
	}

	void SetMovementRequest(const SMovementParams& params);
	void SetRotationRequest(const SRotationParams& params);
	float GetCurrentSpeed() const;
	float GetCurrentSpeedSquared() const;
	Vec3 GetCurrentVelocity() const;

private:
	void Move(float frameTime);
	void Rotate(float frameTime);
	void Stop();

	void SetUpdateActive(bool bActive);

private:
	Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterController = nullptr;

	SMovementParams m_movementParams;
	SRotationParams m_rotationParams;

	Vec3 m_currentVelocity = ZERO;

	bool m_bIsMoving = false;
	bool m_bIsRotating = false;

	bool m_bIsUpdating = true;

};

