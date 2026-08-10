#include "Tools/VoxelToolAdd.h"

#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/VoxelEditingInterface.h"
#include "Subsystems/VoxelInventorySubsystem.h"

bool UVoxelToolAdd::ExecuteTool_Implementation(const FHitResult& HitResult)
{
	if (!HitResult.bBlockingHit)
	{
		return false;
	}

	UVoxelInventorySubsystem* Inventory = GetInventory();
	if (!Inventory)
	{
		return false;
	}

	const EVoxelType VoxelType = Inventory->GetSelectedVoxelType();
	if (!Inventory->HasVoxels(VoxelType))
	{
		return false;
	}

	UWorld* World = GetWorld();
	AGameModeBase* GameMode = World ? World->GetAuthGameMode() : nullptr;
	if (!GameMode || !GameMode->Implements<UVoxelEditingInterface>())
	{
		return false;
	}

	const FVector AddLocation = HitResult.ImpactPoint + HitResult.ImpactNormal * SurfaceOffset;
	if (!IVoxelEditingInterface::Execute_AddVoxel(GameMode, AddLocation, VoxelType))
	{
		return false;
	}

	return Inventory->RemoveVoxels(VoxelType);
}
