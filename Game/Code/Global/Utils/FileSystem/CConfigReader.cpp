#include "StdAfx.h"
#include "CConfigReader.h"
#include <CrySystem/ISystem.h>
#include <CrySystem/File/ICryPak.h>

std::vector<string> CConfigReader::GetFiles(const string& directory, const string& extensionFilter)
{
	std::vector<string> filePaths;
	string prefix = directory + "/";
	string searchPath = prefix + extensionFilter;

	_finddata_t fd;
	intptr_t handle = gEnv->pCryPak->FindFirst(searchPath.c_str(), &fd);

	if (handle == -1) return filePaths;

	do
	{
		if (!(fd.attrib & _A_SUBDIR))
			filePaths.emplace_back(prefix + fd.name);
	} while (gEnv->pCryPak->FindNext(handle, &fd) >= 0);

	gEnv->pCryPak->FindClose(handle);
	return filePaths;
}

std::vector<char> CConfigReader::ReadFile(const char* filePath)
{
	std::vector<char> buffer;

	FILE* pFile = gEnv->pCryPak->FOpen(filePath, "rb");
	if (!pFile)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "CConfigReader: Failed to open file: %s", filePath);
		return buffer;
	}

	gEnv->pCryPak->FSeek(pFile, 0, SEEK_END);
	long fileSize = gEnv->pCryPak->FTell(pFile);
	gEnv->pCryPak->FSeek(pFile, 0, SEEK_SET);

	if (fileSize > 0)
	{
		buffer.resize(static_cast<size_t>(fileSize) + 1);
		size_t bytesRead = gEnv->pCryPak->FReadRaw(buffer.data(), 1, fileSize, pFile);
		buffer[bytesRead] = '\0';
	}

	gEnv->pCryPak->FClose(pFile);
	return buffer;
}