#pragma once


class CUIStringTable;

class IUIStringTableConsumer
{
public:
	virtual ~IUIStringTableConsumer() = default;
	virtual void PushStringTable(const CUIStringTable& table) = 0;
};