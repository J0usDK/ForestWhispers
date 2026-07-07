#pragma once

class ITipsViewListener
{
public:
	virtual ~ITipsViewListener() = default;
	virtual void OnTipsStateChanged(uint32 stringID, bool isVisible) = 0;
};