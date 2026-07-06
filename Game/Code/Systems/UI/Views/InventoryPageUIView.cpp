#include "StdAfx.h"
#include "InventoryPageUIView.h"
#include "Systems/UI/Types/InventoryUIRowType.h"

CInventoryPageUIView::CInventoryPageUIView()
{
	m_pUIElement = gEnv->pFlashUI->GetUIElement("BookUI");
}

void CInventoryPageUIView::OnInventoryDataUpdated(const std::vector<SUIItemData>& itemsSnapshot)
{
	SUIArguments args;

	for (const auto& item : itemsSnapshot)
	{
		args.AddArgument(item.nameStringID);
		args.AddArgument(item.weight);
		args.AddArgument(static_cast<int>(UITypes::EInventoryUIRowType::ROW_ITEM));
	}

	m_pUIElement->CallFunction("Inv_SetItems", args);
}