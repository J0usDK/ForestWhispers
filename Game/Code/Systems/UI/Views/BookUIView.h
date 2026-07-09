#pragma once
#include <CrySystem/Scaleform/IFlashUI.h>
#include "Systems/UI/Listeners/IBookViewListener.h"


class CBookUIView final : public IBookViewListener, public IUIElementEventListener
{
public:
	CBookUIView();
	~CBookUIView();

	void OnBookStateChanged(bool isVisible) override;

	void Reset();
	void OnInit(IUIElement* pSender, IFlashPlayer* pFlashPlayer) override;

private:
	void PushStringTable();

private:
	IUIElement* m_pUIElement = nullptr;
};