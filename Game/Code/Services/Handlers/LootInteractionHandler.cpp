#include "StdAfx.h"
#include "LootInteractionHandler.h"

#include <CryEntitySystem/IEntitySystem.h>

#include "Components/Items/ItemComponent.h"
#include "Components/Inventory/InventoryComponent.h"


bool CLootInteractionHandler::CanInteract(const SInteractionContext& ctx)
{
	CItemComponent* pItemComponent = ctx.target->GetComponent<CItemComponent>();
	if (!pItemComponent) return false;

	IInventory* pInventoryComponent = ctx.actor->GetComponent<CInventoryComponent>();
	if (!pInventoryComponent) return false;

	const CItemInstance* pItemInstance = pItemComponent->GetInstance();
	if (!pItemInstance || !pInventoryComponent->CanAddItem(pItemInstance))
		return false;
	return true;
}

void CLootInteractionHandler::OnInteract(const SInteractionContext& ctx)
{
	CItemComponent* pItemComponent = ctx.target->GetComponent<CItemComponent>();
	IInventory* pInventoryComponent = ctx.actor->GetComponent<CInventoryComponent>();

	if (!pItemComponent || !pInventoryComponent)
		return;

	std::unique_ptr<CItemInstance> pItemInstance = pItemComponent->TakeInstance();
	if (!pItemInstance)
		return;

	if (pInventoryComponent->TryAddItem(pItemInstance))
		gEnv->pEntitySystem->RemoveEntity(ctx.target->GetId());
	else
		pItemComponent->SetInstance(std::move(pItemInstance));
}