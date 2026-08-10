#include "Tools/VoxelToolAdd.h"

#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/VoxelEditingInterface.h"
#include "Subsystems/VoxelInventorySubsystem.h"

bool UVoxelToolAdd::ExecuteTool_Implementation(const FHitResult& HitResult)
{
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
	
	FVector AddLocation;
	if (HitResult.bBlockingHit)
	{
		AddLocation = HitResult.ImpactPoint + HitResult.ImpactNormal * SurfaceOffset;
	}
	else
	{
		AddLocation = HitResult.TraceEnd;
	}
	
	if (!IVoxelEditingInterface::Execute_AddVoxel(GameMode, AddLocation, VoxelType))
	{
		return false;
	}

	return Inventory->RemoveVoxels(VoxelType);
}
