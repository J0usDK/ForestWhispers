#include "StdAfx.h"
#include "FootstepAudioController.h"

#include <CryPhysics/physinterface.h>
#include <Cry3DEngine/ISurfaceType.h>
#include <MaterialEffects/MaterialEffects.h>

namespace
{
	constexpr const char* kMaterialPrefix = "mat_";
	constexpr size_t kMaterialPrefixLen = 4;

	const char* StripMaterialPrefix(const char* name)
	{
		if (name && strncmp(name, kMaterialPrefix, kMaterialPrefixLen) == 0)
			return name + kMaterialPrefixLen;
		return name;
	}
}

void CFootstepAudioController::Initialize(IEntity* pEntity)
{
	m_pEntity = pEntity;

	m_triggerID = CryAudio::StringToId("Play_Player_Footstep");
	m_switchID = CryAudio::StringToId("Surface_Type");
	m_defaultStateID = CryAudio::StringToId("dirt");

	BuildSurfaceSwitchTable();
}

void CFootstepAudioController::BuildSurfaceSwitchTable()
{
	m_surfaceSwitchTable.clear();
	m_surfaceSwitchTable.resize(128);
	ISurfaceTypeEnumerator* pSurfaceEnum = gEnv->p3DEngine->GetMaterialManager()->GetSurfaceTypeManager()->GetEnumerator();

	for (auto pSurfaceType = pSurfaceEnum->GetFirst(); pSurfaceType != nullptr; pSurfaceType = pSurfaceEnum->GetNext())
	{
		const char* name = StripMaterialPrefix(pSurfaceType->GetName());
		m_surfaceSwitchTable[pSurfaceType->GetId()] = CryAudio::StringToId(name);
	}
}

void CFootstepAudioController::ProcessFootstep(const Vec3& pointPosition)
{
	if (!m_pEntity) return;
	;
	IEntityAudioComponent* pAudio = GetOrCreateAudioComponent();
	const int surfaceIdx = GetSurfaceIndexUnderPoint(pointPosition);
	const CryAudio::ControlId stateID = ResolveSwitchState(surfaceIdx);

	if (m_switchID != CryAudio::InvalidControlId && stateID != CryAudio::InvalidControlId && stateID != m_lastStateID)
		pAudio->SetSwitchState(m_switchID, stateID);
	m_lastStateID = stateID;

	if (m_triggerID != CryAudio::InvalidControlId)
		pAudio->ExecuteTrigger(m_triggerID);
}

int CFootstepAudioController::GetSurfaceIndexUnderPoint(const Vec3& pointPosition) const
{
	Vec3 rayStart = pointPosition + Vec3(0, 0, 1.5f);
	Vec3 rayDir(0, 0, -2.0f);

	ray_hit hit;
	IPhysicalEntity* pSkipList[1] = { m_pEntity->GetPhysics() };

	int hits = gEnv->pPhysicalWorld->RayWorldIntersection(rayStart, rayDir, ent_static | ent_terrain, rwi_stop_at_pierceable, &hit, 1, pSkipList, 1);
	return hits > 0 ? hit.surface_idx : -1;
}

CryAudio::ControlId CFootstepAudioController::ResolveSwitchState(int surfaceIdx) const
{
	if (surfaceIdx >= m_surfaceSwitchTable.size())
		return m_defaultStateID;
	return m_surfaceSwitchTable[surfaceIdx];
}

IEntityAudioComponent* CFootstepAudioController::GetOrCreateAudioComponent()
{
	if (!m_pAudio)
		m_pAudio = m_pEntity->GetOrCreateComponent<IEntityAudioComponent>();

	return m_pAudio;
}