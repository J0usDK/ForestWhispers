#pragma once

#include <CryEntitySystem/IEntitySystem.h>

#include "Systems/Items/ItemInstance.h"

class CPhysicalItemFactory
{
public:
	CPhysicalItemFactory() = default;
	~CPhysicalItemFactory() = default;

	void InitEditorItem(IEntity* pTargetEntity, const char* stringID);
	IEntity* SpawnPhysicalItem(std::unique_ptr<CItemInstance> pInstance, const Vec3& position, const Quat& rotation = IDENTITY);

private:
	void InitPhysicalItem(IEntity* pTargetEntity, std::unique_ptr<CItemInstance> pInstance);
};