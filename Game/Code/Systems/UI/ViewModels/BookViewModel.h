#pragma once
#include "Systems/UI/Listeners/IBookViewListener.h"

class CBookViewModel
{
public:
	CBookViewModel() = default;
	~CBookViewModel() = default;

	void Reset();
	void SetListener(IBookViewListener* pListener);

	void ToggleBook();

private:
	IBookViewListener* m_pListener = nullptr;
	bool m_bBookVisible = false;
};