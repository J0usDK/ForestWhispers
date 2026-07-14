#pragma once
#include <CryEntitySystem/IEntity.h>

class CFootstepAudioController
{
public:
	CFootstepAudioController() = default;
	~CFootstepAudioController() = default;

	void Initialize(IEntity* pEntity);

	void ProcessFootstep(const Vec3& pointPosition);

private:
	void BuildSurfaceSwitchTable();
	int GetSurfaceIndexUnderPoint(const Vec3& pointPosition) const;
	CryAudio::ControlId ResolveSwitchState(int surfaceIdx) const;
	IEntityAudioComponent* GetOrCreateAudioComponent();

private:
	IEntity* m_pEntity = nullptr;
	IEntityAudioComponent* m_pAudio = nullptr;

	CryAudio::ControlId m_triggerID = CryAudio::InvalidControlId;
	CryAudio::ControlId m_switchID = CryAudio::InvalidControlId;
	CryAudio::ControlId m_defaultStateID = CryAudio::InvalidControlId;
	CryAudio::ControlId m_lastStateID = CryAudio::InvalidControlId;

	std::vector<CryAudio::ControlId> m_surfaceSwitchTable;
};