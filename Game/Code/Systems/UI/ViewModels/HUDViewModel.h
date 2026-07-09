#pragma once
#include "Systems/UI/Listeners/IHUDViewListener.h"

class CHUDViewModel
{
public:
	CHUDViewModel() = default;
	~CHUDViewModel() = default;

	void Reset();
	void SetListener(IHUDViewListener* pListener);

	void ShowCrosshair();
	void HideCrosshair();

	void ShowHUD();
	void HideHUD();

private:
	IHUDViewListener* m_pListener = nullptr;
	bool m_bCrosshairVisible = false;
	bool m_bHUDVisible = false;
};