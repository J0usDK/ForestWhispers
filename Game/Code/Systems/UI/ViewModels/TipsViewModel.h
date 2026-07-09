#pragma once
#include "Systems/UI/Listeners/ITipsViewListener.h"

class CTipsViewModel
{
public:
	CTipsViewModel() = default;
	~CTipsViewModel() = default;

	void Reset();
	void SetListener(ITipsViewListener* pListener);
	
	void Show(uint32 stringID);
	void Hide();

private:
	void NotifyListener();

private:
	ITipsViewListener* m_pListener = nullptr;
	uint32 m_prompt;
	bool m_bIsVisible = false;
};