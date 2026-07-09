#pragma once
#include "Systems/UI/Listeners/IHUDViewListener.h"
#include "CrySystem/Scaleform/IFlashUI.h"

class CHUDUIView final : public IHUDViewListener
{
public:
	CHUDUIView();
	~CHUDUIView() = default;

	void Reset();
	void OnCrosshairStateChanged(bool isVisible) override;
	void OnHUDStateChanged(bool isVisible) override;

private:
	IUIElement* m_pUIElement = nullptr;
};