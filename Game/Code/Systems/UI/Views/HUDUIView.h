#pragma once
#include "Systems/UI/Listeners/IHUDViewListener.h"
#include "CrySystem/Scaleform/IFlashUI.h"

class CHUDUIView final : public IHUDViewListener
{
public:
	CHUDUIView() = default;
	~CHUDUIView() = default;

	virtual void OnCrosshairStateChanged(bool isVisible) override;
	virtual void OnHUDStateChanged(bool isVisible) override;

private:
	bool Initialize();

private:
	IUIElement* m_pUIElement = nullptr;
};