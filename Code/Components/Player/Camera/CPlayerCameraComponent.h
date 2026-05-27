#pragma once
#include <optional>

#include <DefaultComponents/Cameras/CameraComponent.h>

class CPlayerCameraComponent final : public IEntityComponent
{
public:
	CPlayerCameraComponent() = default;
	virtual ~CPlayerCameraComponent() = default;

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	static void ReflectType(Schematyc::CTypeDesc<CPlayerCameraComponent>& desc)
	{
		desc.SetGUID("{829D009B-9391-4B9D-A636-D019DF350D72}"_cry_guid);
		desc.AddMember(&CPlayerCameraComponent::m_rotationSpeed, 'rs', "rotationspeed", "Rotation Speed", "Mouse sensitivity", 0.0f);
		desc.AddMember(&CPlayerCameraComponent::m_pitchMin, 'pmin', "pitchmin", "Min Pitch", "Maximal look down angle (radians)", 0.0f);
		desc.AddMember(&CPlayerCameraComponent::m_pitchMax, 'pmax', "pitchmax", "Max Pitch", "Maximal look up angle (radians)", 0.0f);
		desc.AddMember(&CPlayerCameraComponent::m_cameraOffset, 'coff', "cameraoffset", "Camera Offset", "Offset from the pivot point", ZERO);
	}

	void ApplyInputDelta(const Vec2& lookDelta);
	void SetPivotPosition(const Vec3& worldPosition);

	void SetBaseFrameRotation(const Quat& worldRotation);

	Quat GetLookOrientation() const;
	float GetAbsoluteYaw() const;
	float GetAbsolutePitch() const;

private:
	void UpdateCameraTransform();

private:
	Cry::DefaultComponents::CCameraComponent* m_pCameraComponent = nullptr;

	struct SCameraState
	{
		float localYaw = 0.0f;
		float localPitch = 0.0f;

		Vec3 pivotPosition = ZERO;
		Quat baseFrameRotation = IDENTITY;
	} m_cameraState;

	float m_rotationSpeed = 0.0f;
	float m_pitchMin = 0.0f;
	float m_pitchMax = 0.0f;
	Vec3 m_cameraOffset = ZERO;
};

