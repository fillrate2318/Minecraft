#include "Tools/VoxelToolRemove.h"

#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/VoxelEditingInterface.h"
#include "Subsystems/VoxelInventorySubsystem.h"

bool UVoxelToolRemove::ExecuteTool_Implementation(const FHitResult& HitResult)
{
	if (!HitResult.bBlockingHit)
	{
		return false;
	}

	UWorld* World = GetWorld();
	AGameModeBase* GameMode = World ? World->GetAuthGameMode() : nullptr;
	if (!GameMode || !GameMode->Implements<UVoxelEditingInterface>())
	{
		return false;
	}

	UVoxelInventorySubsystem* Inventory = GetInventory();
	if (!Inventory)
	{
		return false;
	}

	const FVector RemoveLocation = HitResult.ImpactPoint - HitResult.ImpactNormal * SurfaceOffset;
	const EVoxelType RemovedType = IVoxelEditingInterface::Execute_GetVoxelType(GameMode, RemoveLocation);
	if (RemovedType == EVoxelType::Empty ||
		!IVoxelEditingInterface::Execute_RemoveVoxel(GameMode, RemoveLocation))
	{
		return false;
	}

	return Inventory->AddVoxels(RemovedType);
}
