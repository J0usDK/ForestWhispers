#pragma once
#include <CrySystem/Scaleform/IFlashUI.h>
#include "Systems/UI/Listeners/IInventoryPageViewListener.h"

class CInventoryPageUIView final : public IInventoryPageViewListener
{
public:
	CInventoryPageUIView();
	~CInventoryPageUIView() = default;

	void OnInventoryDataUpdated(const std::vector<SUIItemData>& itemsSnapshot) override;

private:
	IUIElement* m_pUIElement = nullptr;
};