#pragma once
#include "Systems/UI/Listeners/ITipsViewListener.h"
#include <CrySystem/Scaleform/IFlashUI.h>

class CTipsUIView final : public ITipsViewListener, public IUIElementEventListener
{
public:
	CTipsUIView();
	~CTipsUIView();

	void OnTipsStateChanged(uint32 stringID, bool isVisible) override;

	void OnInit(IUIElement* pSender, IFlashPlayer* pFlashPlayer) override;

private:
	void PushStringTable();

private:
	IUIElement* m_pUIElement = nullptr;
};