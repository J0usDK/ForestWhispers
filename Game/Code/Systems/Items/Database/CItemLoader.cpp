#include "StdAfx.h"
#include "CItemLoader.h"
#include "Global/Utils/FileSystem/CConfigReader.h"
#include "CItemParser.h"

#include <CryThreading/IJobManager.h>

CItemLoader::CItemLoader(const string& directoryPath) : m_directoryPath(directoryPath)
{
}

bool CItemLoader::LoadItems(CItemDatabase& database)
{
	std::vector<string> files = CConfigReader::GetFiles(m_directoryPath, "*.json");
	if (files.empty()) return false;

	std::vector<SItemDefinition> parsedItems(files.size());
	JobManager::SJobState jobSyncState;

	for (size_t i = 0; i < files.size(); ++i)
	{
		auto sharedBuffer = std::make_shared<std::vector<char>>(CConfigReader::ReadFile(files[i].c_str()));
		if (sharedBuffer->empty()) continue;

		gEnv->pJobManager->AddLambdaJob("ParseItemConfig", [sharedBuffer, &parsedItems, i]()
		{
			CItemParser::Parse(*sharedBuffer, parsedItems[i]);
		}, JobManager::eRegularPriority, &jobSyncState);
	}
	gEnv->pJobManager->WaitForJob(jobSyncState);

	std::vector<SItemDefinition> validItems;
	validItems.reserve(parsedItems.size());
	for (auto& item : parsedItems)
	{
		if (item.id != 0)
			validItems.push_back(std::move(item));
	}

	database.RegisterDefinitionsBatch(std::move(validItems));
	return true;
}