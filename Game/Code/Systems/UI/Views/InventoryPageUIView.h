#pragma once
#include <CrySystem/Scaleform/IFlashUI.h>
#include "Systems/UI/Listeners/IInventoryPageViewListener.h"
#include "Systems/UI/Listeners/IInventoryPageCommands.h"

class CInventoryPageUIView final : public IUIElementEventListener, public IInventoryPageViewListener
{
public:
	CInventoryPageUIView();
	~CInventoryPageUIView();

	void OnInventoryDataUpdated(const std::vector<SUIItemData>& itemsSnapshot) override;
	void SetCommandListener(IInventoryPageCommands* pCommandListener);

	void OnUIEvent(IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args) override;

private:
	IUIElement* m_pUIElement = nullptr;
	IInventoryPageCommands* m_pCommandListener = nullptr;
};