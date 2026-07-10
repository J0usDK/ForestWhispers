#include "StdAfx.h"
#include "StaminaComponent.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>

namespace
{
	static void RegisterStaminaComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CStaminaComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterStaminaComponent);
}

void CStaminaComponent::Initialize()
{
	Reset();
}

Cry::Entity::EventFlags CStaminaComponent::GetEventMask() const
{
	Cry::Entity::EventFlags flags = Cry::Entity::EEvent::Reset;

	if (m_bIsUpdating)
		flags |= Cry::Entity::EEvent::Update;

	return flags;
}

void CStaminaComponent::SetUpdateActive(bool bActive)
{
	if (m_bIsUpdating == bActive)
		return;

	m_bIsUpdating = bActive;
	m_pEntity->UpdateComponentEventMask(this);
}

void CStaminaComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case Cry::Entity::EEvent::Update:
		{
			float frameTime = gEnv->pTimer->GetFrameTime();
			UpdateRegen(frameTime);
			break;
		}
		case Cry::Entity::EEvent::Reset:
		{
			Reset();
			SetUpdateActive(false);
			break;
		}
	}
}

void CStaminaComponent::UpdateRegen(float frameTime)
{
	if (m_currentRegenDelay > 0.0f)
	{
		m_currentRegenDelay -= frameTime;
		return;
	}

	m_currentStamina += m_regenRate * frameTime;

	if (m_currentStamina >= m_maxStamina)
	{
		m_currentStamina = m_maxStamina;
		SetUpdateActive(false);
	}

	if (m_bIsExhausted && m_currentStamina >= m_exhaustionThreshold)
		m_bIsExhausted = false;
}

void CStaminaComponent::Reset()
{
	m_currentStamina = m_maxStamina;
	m_bIsExhausted = false;
	m_currentRegenDelay = 0.0f;
}

bool CStaminaComponent::CanConsume(float amount) const
{
	return !m_bIsExhausted && m_currentStamina >= amount;
}

void CStaminaComponent::Consume(float amount)
{
	if (amount <= 0.0f)
		return;

	m_currentStamina = std::max(0.0f, m_currentStamina - amount);
	m_currentRegenDelay = m_regenDelay;

	if (m_currentStamina == 0.0f)
		m_bIsExhausted = true;

	SetUpdateActive(true);
}

float CStaminaComponent::GetCurrentStamina() const { return m_currentStamina; }
float CStaminaComponent::GetMaxStamina() const { return m_maxStamina; }
bool CStaminaComponent::IsExhausted() const { return m_bIsExhausted; }