#pragma once
#include <memory>
#include <vector>
#include "Systems/Items/Database/ItemDefinition.h"

class CItem
{
private:
	using TTraitVec = std::vector<std::unique_ptr<IItemTrait>>;

public:
	explicit CItem(TItemDefinitionID defID, uint32 nameStringID);

	uint32 GetNameStringID() const;
	TItemDefinitionID GetDefinitionID() const;

	bool HasTrait(TTraitID traitID) const;
	void AddTrait(std::unique_ptr<IItemTrait> pTrait);

	bool IsEqual(const CItem& pOther) const;

	template<typename T>
	const T* TryGetTrait() const
	{
		static_assert(std::is_base_of_v<IItemTrait, T>, "Type must derive from IItemTrait");

		TTraitVec::const_iterator it = FindTrait(T::ID);
		if (it != m_traits.end())
			return static_cast<const T*>((*it).get());
		return nullptr;
	}

	template<typename T>
	T* TryGetTrait()
	{
		static_assert(std::is_base_of_v<IItemTrait, T>, "Type must derive from IItemTrait");

		TTraitVec::iterator it = FindTrait(T::ID);
		if (it != m_traits.end())
			return static_cast<T*>((*it).get());
		return nullptr;
	}

	template<class F>
	void ForEachTrait(F&& f) const
	{
		for (const auto& t : m_traits)
			f(*t);
	}

	const IItemTrait* TryGetTraitRaw(TTraitID traitID) const;
	IItemTrait* TryGetTraitRaw(TTraitID traitID);


private:
	// Binary search
	TTraitVec::const_iterator FindTrait(TTraitID traitID) const;
	TTraitVec::iterator FindTrait(TTraitID traitID);

private:
	TItemDefinitionID m_defID;
	uint32 m_nameStringID;
	TTraitVec m_traits;
};