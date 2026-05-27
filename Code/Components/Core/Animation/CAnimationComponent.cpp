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
	return Cry::Entity::EEvent::Update | Cry::Entity::EEvent::Reset;
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
	if (!m_pAnimComponent || paramID < 0)
		return;
		
	auto& param = m_motionParams[paramID];
	param.target = value;
	param.interpSpeed = interpSpeed;
	param.type = interpType;

	if (Math::IsNearlyZero(interpSpeed) || Math::IsNearlyZero(param.target - param.current))
	{
		param.current = value;
		param.bIsActive = true;
		m_pAnimComponent->SetMotionParameter(paramID, param.current);
	}
	else
		param.bIsActive = true;
}

void CAnimationComponent::UpdateParameters(float frameTime)
{
	if (!m_pAnimComponent) return;

	for (int i = 0; i < eMotionParamID_COUNT; ++i)
	{
		auto& param = m_motionParams[i];
		if (!param.bIsActive) continue;

		switch (param.type)
		{
			case EParamInterpType::Angle:
			{
				float diff = std::remainderf(param.target - param.current, 2.0f * g_PI);
				param.current += diff * CLAMP(frameTime * param.interpSpeed, 0.0f, 1.0f);
				param.current = std::remainderf(param.current, 2.0f * g_PI);
				break;
			}
			case EParamInterpType::Linear:
			{
				param.current += (param.target - param.current) * CLAMP(frameTime * param.interpSpeed, 0.0f, 1.0f);
				break;
			}
			case EParamInterpType::None:
			{
				param.current = param.target;
				break;
			}
		}

		m_pAnimComponent->SetMotionParameter(static_cast<EMotionParamID>(i), param.current);
		if (Math::IsNearlyZero(param.target - param.current))
		{
			param.current = param.target;
			m_pAnimComponent->SetMotionParameter(static_cast<EMotionParamID>(i), param.current);
			param.bIsActive = false;
		}
	}
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