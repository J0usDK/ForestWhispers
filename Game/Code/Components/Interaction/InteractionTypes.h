#pragma once

enum class EInteractionType
{
	None = 0,
	Item = 1
};

static void ReflectType(Schematyc::CTypeDesc<EInteractionType>& desc)
{
	desc.SetGUID("{09451E98-4E34-41B7-99D3-D14B82DAC1D6}"_cry_guid);
	desc.SetLabel("Interaction Type");

	desc.AddConstant(EInteractionType::None, "None", "None");
	desc.AddConstant(EInteractionType::Item, "Item", "Item");
}