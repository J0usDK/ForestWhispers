#pragma once

class IInventoryPageCommands
{
public:
	virtual ~IInventoryPageCommands() = default;

	virtual void SortByType() = 0;
	virtual void SortByName() = 0;
	virtual void SortByWeight() = 0;
};