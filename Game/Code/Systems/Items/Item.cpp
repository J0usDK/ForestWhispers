#include "Item.h"

CItem::CItem(TItemID itemID) : m_itemID(itemID)
{
	m_traits.reserve(4); // Reserve space for a few traits to avoid frequent reallocations
}

TItemID CItem::GetItemID() const
{
	return m_itemID;
}

void CItem::AddTrait(std::unique_ptr<IItemTrait> pTrait)
{
	if (!pTrait)
		return;

	TTraitID newTraitID = pTrait->GetTraitID();
	TraitVec::iterator it = FindTrait(newTraitID);

	if (it != m_traits.end() && (*it)->GetTraitID() == newTraitID)
		*it = std::move(pTrait);
	else
		m_traits.insert(it, std::move(pTrait));
}

bool CItem::HasTrait(TTraitID traitID) const
{
	TraitVec::const_iterator it = FindTrait(traitID);
	return it != m_traits.end() && (*it)->GetTraitID() == traitID;
}

CItem::TraitVec::const_iterator CItem::FindTrait(TTraitID traitID) const
{
	return std::lower_bound(m_traits.begin(), m_traits.end(), traitID,
		[](const std::unique_ptr<IItemTrait>& trait, TTraitID id)
		{ return trait->GetTraitID() < id; });
}
CItem::TraitVec::iterator CItem::FindTrait(TTraitID traitID)
{
	return std::lower_bound(m_traits.begin(), m_traits.end(), traitID,
		[](const std::unique_ptr<IItemTrait>& trait, TTraitID id)
		{ return trait->GetTraitID() < id; });
}