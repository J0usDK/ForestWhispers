#pragma once

#include <cstdint>

namespace StringUtils
{
	constexpr uint32_t GenerateID(const char* stringID, uint32_t hash = 2166136261u)
	{
		if (!stringID || stringID[0] == '\0')
			return hash;
		return GenerateID(stringID + 1, (hash ^ static_cast<uint32_t>(*stringID)) * 16777619u);
	}
}