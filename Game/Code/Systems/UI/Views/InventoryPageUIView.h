#pragma once
#include <CrySystem/Scaleform/IFlashUI.h>
#include "Systems/UI/Listeners/IInventoryPageViewListener.h"

class CInventoryPageUIView final : public IInventoryPageViewListener, public IUIElementEventListener
{
public:
	CInventoryPageUIView() = default;
	~CInventoryPageUIView() = default;

	virtual void OnInventoryDataUpdated(const std::vector<SUIItemData>& itemsSnapshot) override;

	virtual void OnUIEvent(IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args) override;

private:
	bool Initialize();

private:
	IUIElement* m_pUIElement = nullptr;
	IFlashVariableObject* m_pInventoryUI = nullptr;
};