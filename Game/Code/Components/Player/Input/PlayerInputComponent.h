#pragma once

#include <DefaultComponents/Input/InputComponent.h>

struct SCharacterIntent
{
	Vec2 movement = ZERO;
	Vec2 lookDelta = ZERO;
	bool sprint = false;
	bool interact = false;
};

enum class EInputContext
{
	OnFoot,
	UI
};

class CPlayerInputComponent final : public IEntityComponent, public IActionListener
{
public:
	CPlayerInputComponent() = default;
	virtual ~CPlayerInputComponent();

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	virtual void OnAction(const ActionId& action, int activationMode, float value) override;

	static void ReflectType(Schematyc::CTypeDesc<CPlayerInputComponent>& desc)
	{
		desc.SetGUID("{9A1D6DBC-8F18-49D0-B332-D92B291633B1}"_cry_guid);
	}

	const SCharacterIntent& GetCurrentIntent() const;

	void SwitchContext(EInputContext newContext);
	EInputContext GetInputContext() const;

private:
	void InitializeActionMaps();
	void FlushIntent();

	bool OnMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnMoveBack(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnPitch(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	bool OnInteract(EntityId entityId, const ActionId& actionId, int activationMode, float value);
		
private:
	IActionMapManager* m_pActionMapManager = nullptr;
	TActionHandler<CPlayerInputComponent> m_actionHandler;
	
	SCharacterIntent m_accumulator;
	SCharacterIntent m_frameIntent;

	EInputContext m_currentContext = EInputContext::OnFoot;

	struct SInputAxis
	{
		float inputFwd = 0.0f;
		float inputBck = 0.0f;
		float inputRgt = 0.0f;
		float inputLft = 0.0f;
	} m_inputAxis;
};

