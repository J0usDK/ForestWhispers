#pragma once

class ITipsViewListener
{
public:
	virtual ~ITipsViewListener() = default;
	virtual void OnTipsStateChanged(const char* prompt, bool isVisible) = 0;
};