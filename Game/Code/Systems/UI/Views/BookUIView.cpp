#include "StdAfx.h"
#include "BookUIView.h"

#include "Global/GameEnv.h"
#include "Systems/UI/UIStringTable.h"

CBookUIView::CBookUIView()
{
	m_pUIElement = gEnv->pFlashUI->GetUIElement("BookUI");
	m_pUIElement->SetVisible(true);
	m_pUIElement->AddEventListener(this, "OnInit");
	m_pUIElement->Init();
}

CBookUIView::~CBookUIView()
{
	m_pUIElement->RemoveEventListener(this);
}

void CBookUIView::Reset()
{
	m_pUIElement->CallFunction("CloseBook");
}

void CBookUIView::OnInit(IUIElement* pSender, IFlashPlayer* pFlashPlayer)
{
	PushStringTable();
}

void CBookUIView::OnBookStateChanged(bool isVisible)
{
	if (isVisible)
		m_pUIElement->CallFunction("OpenBook");
	else
		m_pUIElement->CallFunction("CloseBook");
}

void CBookUIView::PushStringTable()
{
	const CUIStringTable* pStringTable = gGameEnv->pUIStringTable;

	SUIArguments args;
	pStringTable->ForEach([&args](uint32 id, const char* text) {
		args.AddArgument(id);
		args.AddArgument(text);
	});

	m_pUIElement->CallFunction("SetStringTable", args);
}