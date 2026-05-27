#pragma once

#include <optional>
#include <DefaultComponents/Geometry/AdvancedAnimationComponent.h>

enum class EParamInterpType
{
	Linear,
	Angle,
	None
};

class CAnimationComponent final : public IEntityComponent
{
public:
	CAnimationComponent() = default;
	virtual ~CAnimationComponent() = default;

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	static void ReflectType(Schematyc::CTypeDesc<CAnimationComponent>& desc)
	{
		desc.SetGUID("{3EA4F033-3351-4F98-9A03-F6BBC998D450}"_cry_guid);
	}

	void QueueFragment(const char* fragmentName);
	void SetMotionParameter(EMotionParamID paramID, float value, float interpSpeed = 0.0f, EParamInterpType interpType = EParamInterpType::None);
	void UpdateParameters(float frameTime);

	std::optional<int16> GetBoneID(const char* boneName) const;
	std::optional<Vec3> GetBoneWorldPosition(int16 boneID) const;

private:
	Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAnimComponent = nullptr;

	struct SParamState
	{
		float current = 0.0f;
		float target = 0.0f;
		float interpSpeed = 0.0f;
		EParamInterpType type = EParamInterpType::None;
		bool bIsActive = false;
	};

	std::array<SParamState, eMotionParamID_COUNT> m_motionParams;
};