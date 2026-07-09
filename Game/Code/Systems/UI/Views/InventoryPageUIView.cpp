#include "StdAfx.h"
#include "InventoryPageUIView.h"
#include "Systems/UI/Types/UIItemData.h"
#include "Systems/UI/Types/InventoryUIRowType.h"

CInventoryPageUIView::CInventoryPageUIView()
{
	m_pUIElement = gEnv->pFlashUI->GetUIElement("BookUI");
	m_pUIElement->AddEventListener(this, "OnSortType");
	m_pUIElement->AddEventListener(this, "OnSortName");
	m_pUIElement->AddEventListener(this, "OnSortWeight");
	m_pUIElement->Init();
}

CInventoryPageUIView::~CInventoryPageUIView()
{
	m_pUIElement->RemoveEventListener(this);
}

void CInventoryPageUIView::Reset()
{
	SUIArguments args;
	m_pUIElement->CallFunction("Inv_SetItems", args);
}

void CInventoryPageUIView::SetCommandListener(IInventoryPageCommands* pCommandListener)
{
	m_pCommandListener = pCommandListener;
}

void CInventoryPageUIView::OnInventoryDataUpdated(const std::vector<SUIItemData>& itemsSnapshot)
{
	SUIArguments args;

	for (const auto& item : itemsSnapshot)
	{
		args.AddArgument(item.nameStringID);
		args.AddArgument(item.weight);
		args.AddArgument(static_cast<int>(item.rowType));
	}

	m_pUIElement->CallFunction("Inv_SetItems", args);
}

void CInventoryPageUIView::OnUIEvent(IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args)
{
	if (!m_pCommandListener) return;

	if (strcmp(event.sDisplayName, "OnSortName") == 0)
		m_pCommandListener->SortByName();
	else if (strcmp(event.sDisplayName, "OnSortWeight") == 0)
		m_pCommandListener->SortByWeight();
	else if (strcmp(event.sDisplayName, "OnSortType") == 0)
		m_pCommandListener->SortByType();
}