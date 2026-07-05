#pragma once

class IBookViewListener
{
public:
	virtual ~IBookViewListener() = default;

	virtual void OnBookStateChanged(bool isVisible) = 0;
};