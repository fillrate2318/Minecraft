#include "Subsystems/VoxelInventorySubsystem.h"

bool UVoxelInventorySubsystem::AddVoxels(const EVoxelType VoxelType, const int32 Amount)
{
	if (VoxelType == EVoxelType::Empty || Amount <= 0)
	{
		return false;
	}

	int32& Count = VoxelCounts.FindOrAdd(VoxelType);
	Count += Amount;
	OnInventoryChanged.Broadcast(VoxelType, Count);
	return true;
}

bool UVoxelInventorySubsystem::RemoveVoxels(const EVoxelType VoxelType, const int32 Amount)
{
	if (!HasVoxels(VoxelType, Amount))
	{
		return false;
	}

	int32& Count = VoxelCounts.FindChecked(VoxelType);
	Count -= Amount;
	OnInventoryChanged.Broadcast(VoxelType, Count);
	return true;
}

int32 UVoxelInventorySubsystem::GetVoxelCount(const EVoxelType VoxelType) const
{
	const int32* Count = VoxelCounts.Find(VoxelType);
	return Count ? *Count : 0;
}

bool UVoxelInventorySubsystem::HasVoxels(const EVoxelType VoxelType, const int32 Amount) const
{
	return VoxelType != EVoxelType::Empty && Amount > 0 && GetVoxelCount(VoxelType) >= Amount;
}

bool UVoxelInventorySubsystem::SelectVoxelType(const EVoxelType VoxelType)
{
	if (VoxelType == EVoxelType::Empty)
	{
		return false;
	}

	if (SelectedVoxelType != VoxelType)
	{
		SelectedVoxelType = VoxelType;
		OnSelectedVoxelTypeChanged.Broadcast(SelectedVoxelType);
	}

	return true;
}

bool UVoxelInventorySubsystem::Increment()
{
	switch (SelectedVoxelType)
	{
	case EVoxelType::Snow:
		return SelectVoxelType(EVoxelType::Grass);
	case EVoxelType::Grass:
		return SelectVoxelType(EVoxelType::Rock);
	case EVoxelType::Rock:
		return SelectVoxelType(EVoxelType::Snow);
	default:
		return SelectVoxelType(EVoxelType::Snow);
	}
}

bool UVoxelInventorySubsystem::Decrement()
{
	switch (SelectedVoxelType)
	{
	case EVoxelType::Snow:
		return SelectVoxelType(EVoxelType::Rock);
	case EVoxelType::Grass:
		return SelectVoxelType(EVoxelType::Snow);
	case EVoxelType::Rock:
		return SelectVoxelType(EVoxelType::Grass);
	default:
		return SelectVoxelType(EVoxelType::Rock);
	}
}
