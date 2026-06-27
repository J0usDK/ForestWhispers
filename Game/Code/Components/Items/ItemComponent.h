#pragma once
#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/Utils/SharedString.h>

#include "Systems/Items/ItemInstance.h"

class CItemComponent final : public IEntityComponent
{
public:
	CItemComponent() = default;
	virtual ~CItemComponent() = default;

	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	static void ReflectType(Schematyc::CTypeDesc<CItemComponent>& desc)
	{
		desc.SetGUID("{6528EC5E-4A1E-458C-A873-DAD11AE10A58}"_cry_guid);
		
		desc.SetLabel("Physical Item");
		desc.AddMember(&CItemComponent::m_editorStringID, 'itid', "ItemID", "Item String ID", "An id for item", "");
	}

	void SetInstance(std::unique_ptr<CItemInstance> pInstance);
	std::unique_ptr<CItemInstance> TakeInstance();

	const CItemInstance* GetInstance() const;

private:
	void Physicalize();
	void Unphysicalize();

	Schematyc::CSharedString m_editorStringID;
	std::unique_ptr<CItemInstance> m_pInstance;
};