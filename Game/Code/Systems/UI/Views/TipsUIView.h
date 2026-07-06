#pragma once
#include "Systems/UI/Listeners/ITipsViewListener.h"
#include <CrySystem/Scaleform/IFlashUI.h>

class CTipsUIView final : public ITipsViewListener
{
public:
	CTipsUIView();
	~CTipsUIView() = default;

	virtual void OnTipsStateChanged(const char* prompt, bool isVisible) override;

private:
	IUIElement* m_pUIElement = nullptr;
};