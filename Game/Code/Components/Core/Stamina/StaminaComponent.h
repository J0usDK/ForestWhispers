#pragma once
#include "IStamina.h"
#include <CryEntitySystem/IEntity.h>

class CStaminaComponent : public IEntityComponent, public IStamina
{
public:
	CStaminaComponent() = default;
	virtual ~CStaminaComponent() = default;

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	static void ReflectType(Schematyc::CTypeDesc<CStaminaComponent>& desc)
	{
		desc.SetGUID("{14EF3898-0DED-432E-8642-6610C83702B1}"_cry_guid);

		desc.SetLabel("Stamina");
		desc.SetDescription("Handles stamina pool, regeneration, and exhaustion logic");

		desc.AddMember(&CStaminaComponent::m_maxStamina, 'maxs', "MaxStamina", "Max Stamina", "Maximum stamina points", 0.0f);
		desc.AddMember(&CStaminaComponent::m_regenRate, 'regr', "RegenRate", "Regeneration Rate", "Stamina recovered per second", 0.0f);
		desc.AddMember(&CStaminaComponent::m_regenDelay, 'regd', "RegenDelay", "Regeneration Delay", "Seconds to wait after consumption before regeneration", 0.0f);
		desc.AddMember(&CStaminaComponent::m_exhaustionThreshold, 'exht', "ExhaustThreshold", "Exhaustion Threshold", "Minimum stamina required to recover from exhausted state", 0.0f);
	}

	bool CanConsume(float amount) const override;
	void Consume(float amount) override;
	float GetCurrentStamina() const override;
	float GetMaxStamina() const override;
	bool IsExhausted() const override;

private:
	void SetUpdateActive(bool bActive);
	void UpdateRegen(float frameTime);
	void Reset();

private:
	float m_maxStamina = 0.0f;
	float m_regenRate = 0.0f;
	float m_regenDelay = 0.0f;
	float m_exhaustionThreshold = 0.0f;

	float m_currentStamina = 0.0f;
	float m_currentRegenDelay = 0.0f;

	bool m_bIsExhausted = false;
	bool m_bIsUpdating = false;
};