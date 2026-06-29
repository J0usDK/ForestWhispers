#pragma once

class IHUDViewListener
{
public:
	virtual ~IHUDViewListener() = default;

	virtual void OnCrosshairStateChanged(bool isVisible) = 0;
	virtual void OnHUDStateChanged(bool isVisible) = 0;
};