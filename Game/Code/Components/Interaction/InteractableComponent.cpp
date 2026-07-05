#include "StdAfx.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>

#include "InteractableComponent.h"
#include "Global/Utils/StringUtils.h"

namespace
{
	static void RegisterInteractableComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CInteractableComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterInteractableComponent);
}

void CInteractableComponent::Initialize()
{
	if (!m_editorStringKey.empty())
		SetInteractionStringKey(m_editorStringKey.c_str());
}

void CInteractableComponent::SetInteractionType(EInteractionType type)
{
	m_type = type;
}

EInteractionType CInteractableComponent::GetInteractionType() const
{
	return m_type;
}

void CInteractableComponent::SetInteractionStringKey(const char* key)
{
	m_interactionStringKey = StringUtils::GenerateID(key);
}

uint32 CInteractableComponent::GetInteractionStringKey() const
{
	return m_interactionStringKey;
}