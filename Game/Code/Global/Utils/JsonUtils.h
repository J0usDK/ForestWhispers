#pragma once

#include <CryString/CryString.h>
#include "Global/Wrappers/RapidJsonWrapper.h"

namespace JsonUtils
{
	inline bool TryGetFloat(const rapidjson::Value& node, const char* key, float& outValue)
	{
		auto it = node.FindMember(key);
		if (it != node.MemberEnd() && it->value.IsNumber())
		{
			outValue = it->value.GetFloat();
			return true;
		}
		return false;
	}

	inline bool TryGetInt(const rapidjson::Value& node, const char* key, int& outValue)
	{
		auto it = node.FindMember(key);
		if (it != node.MemberEnd() && it->value.IsInt())
		{
			outValue = it->value.GetInt();
			return true;
		}
		return false;
	}

	inline bool TryGetString(const rapidjson::Value& node, const char* key, string& outValue)
	{
		auto it = node.FindMember(key);
		if (it != node.MemberEnd() && it->value.IsString())
		{
			outValue = it->value.GetString();
			return true;
		}
		return false;
	}
}