#pragma once

#include <vector>

class CConfigReader
{
public:
	CConfigReader() = delete;

	static std::vector<string> GetFiles(const string& directory, const string& extensionFilter);
	static std::vector<char> ReadFile(const char* filePath);
};