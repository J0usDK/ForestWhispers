#pragma once

#include <cstdint>
#include <CryCore/CryCrc32.h>

namespace StringUtils
{
	constexpr uint64 GenerateID(const char* stringID, uint64 hash = 14695981039346656037ull)
	{
		if (!stringID || stringID[0] == '\0')
			return hash;
		return GenerateID(stringID + 1, (hash ^ static_cast<uint64>(*stringID)) * 1099511628211ull);
	}
}