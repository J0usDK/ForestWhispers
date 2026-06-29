#pragma once

#include <CryString/CryString.h>

#include "Systems/UI/Listeners/ITipsViewListener.h"

class CTipsViewModel
{
public:
	CTipsViewModel() = default;
	~CTipsViewModel() = default;

	void SetListener(ITipsViewListener* pListener);
	
	void Show(const char* text);
	void Hide();

private:
	void NotifyListener();

private:
	ITipsViewListener* m_pListener = nullptr;
	string m_prompt = "";
	bool m_isVisible = false;
};