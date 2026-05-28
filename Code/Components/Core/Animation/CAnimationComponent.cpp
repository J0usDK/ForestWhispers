#include "StdAfx.h"
#include "CAnimationComponent.h"
#include "Components/Core/Global/Utils/Math.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>

namespace
{
	static void RegisterAnimationComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CAnimationComponent));
		}
	}
	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterAnimationComponent);
}

void CAnimationComponent::Initialize()
{
	m_pAnimComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAdvancedAnimationComponent>();
}

Cry::Entity::EventFlags CAnimationComponent::GetEventMask() const
{
	Cry::Entity::EventFlags flags = Cry::Entity::EEvent::Reset;

	if (m_bIsUpdating)
		flags |= Cry::Entity::EEvent::Update;

	return flags;
}

void CAnimationComponent::SetUpdateActive(bool bActive)
{
	if (m_bIsUpdating == bActive)
		return;

	m_bIsUpdating = bActive;
	m_pEntity->UpdateComponentEventMask(this);
}

void CAnimationComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case Cry::Entity::EEvent::Update:
		{
			float frameTime = gEnv->pTimer->GetFrameTime();
			UpdateParameters(frameTime);
			break;
		}
		case Cry::Entity::EEvent::Reset:
		{
			m_motionParams = {};
			SetUpdateActive(false);
			break;
		}
	}
}

void CAnimationComponent::QueueFragment(const char* fragmentName)
{
	if (m_pAnimComponent)
		m_pAnimComponent->QueueFragment(fragmentName);
}

void CAnimationComponent::SetMotionParameter(EMotionParamID paramID, float value, float interpSpeed, EParamInterpType interpType)
{
	if (!m_pAnimComponent || paramID < 0 || paramID >= eMotionParamID_COUNT)
		return;
		
	auto& param = m_motionParams[paramID];
	param.target = value;
	param.interpSpeed = interpSpeed;
	param.type = interpType;

	float diff = 0.0f;
	if (interpType == EParamInterpType::Angle)
		diff = std::remainderf(param.target - param.current, 2.0f * g_PI);
	else
		diff = param.target - param.current;

	if (Math::IsNearlyZero(interpSpeed) || Math::IsNearlyZero(diff))
	{
		param.current = value;
		param.bIsActive = false;
		m_pAnimComponent->SetMotionParameter(paramID, param.current);
	}
	else
	{
		param.bIsActive = true;
		SetUpdateActive(true);
	}
}

void CAnimationComponent::UpdateParameters(float frameTime)
{
	if (!m_pAnimComponent) return;

	bool bAnyActive = false;

	for (int i = 0; i < eMotionParamID_COUNT; ++i)
	{
		auto& param = m_motionParams[i];
		if (!param.bIsActive) continue;

		const float t = CLAMP(frameTime * param.interpSpeed, 0.0f, 1.0f);
		float diff = 0.0f;

		switch (param.type)
		{
			case EParamInterpType::Angle:
			{
				diff = std::remainderf(param.target - param.current, 2.0f * g_PI);
				param.current += diff * t;
				param.current = std::remainderf(param.current, 2.0f * g_PI);
				break;
			}
			case EParamInterpType::Linear:
			{
				diff = param.target - param.current;
				param.current += diff * t;
				break;
			}
			case EParamInterpType::None:
			{
				param.current = param.target;
				break;
			}
		}

		if (Math::IsNearlyZero(diff))
		{
			param.current = param.target;
			param.bIsActive = false;
		}
		else
			bAnyActive = true;

		m_pAnimComponent->SetMotionParameter(static_cast<EMotionParamID>(i), param.current);
	}

	if (!bAnyActive)
		SetUpdateActive(false);
}

std::optional<int16> CAnimationComponent::GetBoneID(const char* boneName) const
{
	if (!m_pAnimComponent)
		return std::nullopt;

	if (ICharacterInstance* pCharacter = m_pAnimComponent->GetCharacter())
	{
		int16 boneID = pCharacter->GetIDefaultSkeleton().GetJointIDByName(boneName);
		if (boneID >= 0)
			return boneID;
	}

	return std::nullopt;
}

std::optional<Vec3> CAnimationComponent::GetBoneWorldPosition(int16 boneID) const
{
	if (!m_pAnimComponent || boneID < 0)
		return std::nullopt;

	if (ICharacterInstance* pCharacter = m_pAnimComponent->GetCharacter())
	{
		if (ISkeletonPose* pSkeletonPose = pCharacter->GetISkeletonPose())
		{
			QuatT boneTransform = pSkeletonPose->GetAbsJointByID(boneID);
			Matrix34 worldMatrix = m_pEntity->GetWorldTM() * m_pAnimComponent->GetTransformMatrix() * Matrix34(boneTransform);
			return worldMatrix.GetTranslation();
		}
	}
	return std::nullopt;
}