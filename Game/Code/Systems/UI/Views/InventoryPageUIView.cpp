#include "StdAfx.h"
#include "InventoryPageUIView.h"
#include "Systems/UI/Types/InventoryUIRowType.h"

bool CInventoryPageUIView::Initialize()
{
	if (gEnv->pFlashUI)
		m_pUIElement = gEnv->pFlashUI->GetUIElement("BookUI");

	if (!m_pUIElement)
		return false;
	return true;
}

void CInventoryPageUIView::OnInventoryDataUpdated(const std::vector<SUIItemData>& itemsSnapshot)
{
	if (!m_pInventoryUI && !Initialize())
		return;

	SUIArguments args;

	for (const auto& item : itemsSnapshot)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Item: %u, Weight: %f", item.nameStringID, item.weight);
		args.AddArgument(item.nameStringID);
		args.AddArgument(item.weight);
		args.AddArgument(static_cast<int>(UITypes::EInventoryUIRowType::ROW_ITEM));
	}

	bool success = m_pUIElement->CallFunction("Inv_SetItems", args);
	if (success)
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Inv_SetItems invoked successfully.");
	else
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Inv_SetItems invoked unsuccessfully.");
}

void CInventoryPageUIView::OnUIEvent(IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args)
{
	if (event.sDisplayName == "UI_OnSortName")
	{
		// Handle sort by name event
	}
	else if (event.sDisplayName == "UI_OnSortWeight")
	{
		// Handle sort by weight event
	}
}