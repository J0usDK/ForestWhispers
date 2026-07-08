#include "StdAfx.h"
#include "Item.h"

CItem::CItem(const SItemDefinition* pDefinition, uint32 nameStringID) : m_pDefinition(pDefinition), m_nameStringID(nameStringID)
{
	m_traits.reserve(4); // Reserve space for a few traits to avoid frequent reallocations
}

uint32 CItem::GetNameStringID() const
{
	return m_nameStringID;
}

TItemDefinitionID CItem::GetDefinitionID() const
{
	return m_pDefinition->id;
}

const SItemDefinition* CItem::GetDefinition() const
{
	return m_pDefinition;
}

void CItem::AddTrait(std::unique_ptr<IItemTrait> pTrait)
{
	if (!pTrait)
		return;

	TTraitID newTraitID = pTrait->GetTraitID();
	auto it = FindTrait(newTraitID);

	if (it != m_traits.end())
		*it = std::move(pTrait);
	else
		m_traits.insert(it, std::move(pTrait));
}

bool CItem::HasTrait(TTraitID traitID) const
{
	TTraitVec::const_iterator it = FindTrait(traitID);
	return it != m_traits.end() && (*it)->GetTraitID() == traitID;
}

CItem::TTraitVec::const_iterator CItem::FindTrait(TTraitID traitID) const
{
	return std::lower_bound(m_traits.begin(), m_traits.end(), traitID,
		[](const std::unique_ptr<IItemTrait>& trait, TTraitID id)
		{ return trait->GetTraitID() < id; });
}
CItem::TTraitVec::iterator CItem::FindTrait(TTraitID traitID)
{
	return std::lower_bound(m_traits.begin(), m_traits.end(), traitID,
		[](const std::unique_ptr<IItemTrait>& trait, TTraitID id)
		{ return trait->GetTraitID() < id; });
}

bool CItem::IsEqual(const CItem& pOther) const
{
	if (m_pDefinition != pOther.m_pDefinition) return false;
	if (m_traits.size() != pOther.m_traits.size()) return false;

	for (const auto& pTrait : m_traits)
	{
		const IItemTrait* pOtherTrait = pOther.TryGetTraitRaw(pTrait->GetTraitID());
		if (!pOtherTrait || !pTrait->IsEqual(pOtherTrait))
			return false;
	}
	return true;
}

const IItemTrait* CItem::TryGetTraitRaw(TTraitID traitID) const
{
	auto it = FindTrait(traitID);
	if (it != m_traits.end())
		return (*it).get();
	return nullptr;
}

IItemTrait* CItem::TryGetTraitRaw(TTraitID traitID)
{
	auto it = FindTrait(traitID);
	if (it != m_traits.end())
		return (*it).get();
	return nullptr;
}