#pragma once
#include <CrySystem/Scaleform/IFlashUI.h>
#include "Systems/UI/Listeners/IBookViewListener.h"

class CBookUIView final : public IBookViewListener
{
public:
	CBookUIView();
	~CBookUIView() = default;

	virtual void OnBookStateChanged(bool isVisible) override;

private:
	IUIElement* m_pUIElement = nullptr;
};