#include "StdAfx.h"
#include "PlayerInputComponent.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>

namespace
{
	static void RegisterPlayerInputComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerInputComponent));
		}
	}
	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerInputComponent);
}

void CPlayerInputComponent::Initialize()
{
	m_pActionMapManager = gEnv->pGameFramework->GetIActionMapManager();

	m_pActionMapManager->InitActionMaps("libs/config/defaultprofile.xml");
	m_pActionMapManager->Enable(true);
	m_pActionMapManager->AddExtraActionListener(this);

	SwitchContext(EInputContext::OnFoot);
	InitializeActionMaps();
}

void CPlayerInputComponent::InitializeActionMaps()
{
	m_actionHandler.AddHandler(ActionId("moveforward"), &CPlayerInputComponent::OnMoveForward);
	m_actionHandler.AddHandler(ActionId("moveback"), &CPlayerInputComponent::OnMoveBack);

	m_actionHandler.AddHandler(ActionId("moveright"), &CPlayerInputComponent::OnMoveRight);
	m_actionHandler.AddHandler(ActionId("moveleft"), &CPlayerInputComponent::OnMoveLeft);

	m_actionHandler.AddHandler(ActionId("sprint"), &CPlayerInputComponent::OnSprint);

	m_actionHandler.AddHandler(ActionId("yaw"), &CPlayerInputComponent::OnYaw);
	m_actionHandler.AddHandler(ActionId("pitch"), &CPlayerInputComponent::OnPitch);

	m_actionHandler.AddHandler(ActionId("interact"), &CPlayerInputComponent::OnInteract);
}

CPlayerInputComponent::~CPlayerInputComponent()
{
	if (gEnv && gEnv->pGameFramework)
		if (IActionMapManager* pAMM = gEnv->pGameFramework->GetIActionMapManager())
			pAMM->RemoveExtraActionListener(this);
}

Cry::Entity::EventFlags CPlayerInputComponent::GetEventMask() const
{
	return Cry::Entity::EEvent::Update | Cry::Entity::EEvent::Reset;
}

void CPlayerInputComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case Cry::Entity::EEvent::Update:
			m_accumulator.movement.y = (m_inputAxis.inputFwd - m_inputAxis.inputBck);
			m_accumulator.movement.x = (m_inputAxis.inputRgt - m_inputAxis.inputLft);

			m_frameIntent = m_accumulator;

			m_accumulator.lookDelta = ZERO;
			m_accumulator.interact = false;
			break;
		case Cry::Entity::EEvent::Reset:
			FlushIntent();
			SwitchContext(EInputContext::OnFoot);
			break;
	}
}

void CPlayerInputComponent::FlushIntent()
{
	m_accumulator = {};
	m_frameIntent = {};
	m_inputAxis = {};
}

void CPlayerInputComponent::SwitchContext(EInputContext newContext)
{
	m_currentContext = newContext;
	FlushIntent();

	m_pActionMapManager->EnableActionMap("player", (m_currentContext == EInputContext::OnFoot));
	m_pActionMapManager->EnableActionMap("ui", (m_currentContext == EInputContext::UI));
}

const SCharacterIntent& CPlayerInputComponent::GetCurrentIntent() const { return m_frameIntent; }
EInputContext CPlayerInputComponent::GetInputContext() const { return m_currentContext; }

void CPlayerInputComponent::OnAction(const ActionId& action, int activationMode, float value)
{
	m_actionHandler.Dispatch(this, m_pEntity->GetId(), action, activationMode, value);
}

bool CPlayerInputComponent::OnMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_inputAxis.inputFwd = value;
	return true;
}

bool CPlayerInputComponent::OnMoveBack(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_inputAxis.inputBck = value;
	return true;
}

bool CPlayerInputComponent::OnMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_inputAxis.inputRgt = value;
	return true;
}

bool CPlayerInputComponent::OnMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_inputAxis.inputLft = value;
	return true;
}

bool CPlayerInputComponent::OnSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_accumulator.sprint = (value > 0.0f);
	return true;
}

bool CPlayerInputComponent::OnYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_accumulator.lookDelta.y -= value;
	return true;
}

bool CPlayerInputComponent::OnPitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_accumulator.lookDelta.x -= value;
	return true;
}

bool CPlayerInputComponent::OnInteract(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		m_accumulator.interact = true;
	return true;
}