#include "StdAfx.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryPhysics/physinterface.h>

#include "Global/GameEnv.h"
#include "Systems/Items/Database/ItemDatabase.h"
#include "Systems/Items/Factory/PhysicalItemFactory.h"
#include "ItemComponent.h"

namespace
{
	static void RegisterItemComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CItemComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterItemComponent);
}

void CItemComponent::Initialize()
{
	if (!m_editorStringID.empty())
		gGameEnv->pPhysicalItemFactory->InitEditorItem(m_pEntity, m_editorStringID.c_str());
}

Cry::Entity::EventFlags CItemComponent::GetEventMask() const
{
	return Cry::Entity::EEvent::Reset;
}

void CItemComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case Cry::Entity::EEvent::Reset:
			if (!m_editorStringID.empty())
				gGameEnv->pPhysicalItemFactory->InitEditorItem(m_pEntity, m_editorStringID.c_str());
			break;
	}
}

void CItemComponent::SetInstance(std::unique_ptr<CItemInstance> pInstance)
{
	if (!pInstance) return;

	m_pInstance = std::move(pInstance);

	const SItemDefinition* pDef = gGameEnv->pItemDatabase->GetItemDefinition(m_pInstance->GetItemData().GetDefinitionID());

	m_pEntity->LoadGeometry(0, pDef->geometryPath.c_str());
	Physicalize();
}

std::unique_ptr<CItemInstance> CItemComponent::TakeInstance()
{
	m_pEntity->FreeSlot(0);
	Unphysicalize();
	return std::move(m_pInstance);
}

const CItemInstance* CItemComponent::GetInstance() const
{
	return m_pInstance.get();
}

void CItemComponent::Physicalize()
{
	SEntityPhysicalizeParams physicParams;
	physicParams.type = PE_RIGID;
	physicParams.mass = 2.0f;

	m_pEntity->Physicalize(physicParams);
}

void CItemComponent::Unphysicalize()
{
	SEntityPhysicalizeParams physicParams;
	physicParams.type = PE_NONE;

	m_pEntity->Physicalize(physicParams);
}