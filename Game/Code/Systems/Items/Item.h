#pragma once
#include "Systems/Items/Traits/Base/IItemTrait.h"
#include <memory>
#include <vector>

using TItemID = uint32_t;

class CItem
{
private:
	using TraitVec = std::vector<std::unique_ptr<IItemTrait>>;

public:
	explicit CItem(TItemID itemID);

	TItemID GetItemID() const;

	bool HasTrait(TTraitID traitID) const;
	void AddTrait(std::unique_ptr<IItemTrait> pTrait);

	template<typename T>
	T* GetTrait(TTraitID traitID) const
	{
		TraitVec::const_iterator it = FindTrait(traitID);
		if (it != m_traits.end() && (*it)->GetTraitID() == traitID)
			return static_cast<T*>((*it).get());
		return nullptr;
	}


private:
	// Binary search
	TraitVec::const_iterator FindTrait(TTraitID traitID) const;
	TraitVec::iterator FindTrait(TTraitID traitID);

private:
	TItemID m_itemID;
	TraitVec m_traits;
};