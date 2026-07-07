#include "StdAfx.h"
#include "TipsUIView.h"

#include "Global/GameEnv.h"
#include "Systems/UI/UIStringTable.h"

CTipsUIView::CTipsUIView()
{
	m_pUIElement = gEnv->pFlashUI->GetUIElement("worldTooltip");
	m_pUIElement->SetVisible(true);
	m_pUIElement->AddEventListener(this, "OnInit");
}

CTipsUIView::~CTipsUIView()
{
	m_pUIElement->RemoveEventListener(this);
}

void CTipsUIView::OnInit(IUIElement* pSender, IFlashPlayer* pFlashPlayer)
{
	PushStringTable();
}

void CTipsUIView::OnTipsStateChanged(uint32 stringID, bool isVisible)
{
	if (isVisible)
	{
		SUIArguments args;
		args.AddArgument(stringID);
		m_pUIElement->CallFunction("Show", args);
	}
	else
		m_pUIElement->CallFunction("Hide");
}

void CTipsUIView::PushStringTable()
{
	const CUIStringTable* pStringTable = gGameEnv->pUIStringTable;

	SUIArguments args;
	pStringTable->ForEach([&args](uint32 id, const char* text) {
		args.AddArgument(id);
		args.AddArgument(text);
		});

	m_pUIElement->CallFunction("SetStringTable", args);
}