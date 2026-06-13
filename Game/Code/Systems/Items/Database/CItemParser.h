#pragma once

#include "CItemDatabase.h"

class CItemParser
{
public:
	CItemParser() = delete;

	static bool Parse(std::vector<char>& rawBuffer, SItemDefinition& outDef);
};