#include "StdAfx.h"

#include "PhysicalItemFactory.h"
#include "Global/Utils/StringUtils.h"
#include "Global/GameEnv.h"
#include "Systems/Items/Factory/ItemFactory.h"
#include "Systems/Items/Database/ItemDefinition.h"
#include "Components/Interaction/Types/InteractionTypes.h"
#include "Components/Interaction/InteractableComponent.h"
#include "Components/Items/ItemComponent.h"

void CPhysicalItemFactory::InitEditorItem(IEntity* pTargetEntity, const char* stringID)
{
	if (!pTargetEntity || !stringID || stringID[0] == '\0') return;

	TItemDefinitionID defID = StringUtils::GenerateID(stringID);
	std::unique_ptr<CItemInstance> pInstance = gGameEnv->pItemFactory->CreateInstance(defID);

	if (!pInstance) return;

	InitPhysicalItem(pTargetEntity, std::move(pInstance));
}

IEntity* CPhysicalItemFactory::SpawnPhysicalItem(std::unique_ptr<CItemInstance> pInstance, const Vec3& position, const Quat& rotation)
{
	if (!pInstance) return nullptr;

	SEntitySpawnParams spawnParams;
	spawnParams.sName = "DroppedItem";
	spawnParams.vPosition = position;
	spawnParams.qRotation = rotation;

	IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams);
	if (!pEntity) return nullptr;

	InitPhysicalItem(pEntity, std::move(pInstance));

	return pEntity;
}

void CPhysicalItemFactory::InitPhysicalItem(IEntity* pTargetEntity, std::unique_ptr<CItemInstance> pInstance)
{
	auto* pItem = pTargetEntity->GetOrCreateComponent<CItemComponent>();
	pItem->SetInstance(std::move(pInstance));

	auto* pInteractable = pTargetEntity->GetOrCreateComponent<CInteractableComponent>();
	pInteractable->SetInteractionType(EInteractionType::Item);
}