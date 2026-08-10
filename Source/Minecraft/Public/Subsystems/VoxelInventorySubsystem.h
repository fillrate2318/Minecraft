#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Voxels/Data/VoxelData.h"
#include "VoxelInventorySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnVoxelInventoryChanged, EVoxelType, VoxelType, int32, NewCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnSelectedVoxelTypeChanged, EVoxelType, VoxelType);

UCLASS()
class MINECRAFT_API UVoxelInventorySubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Voxel Inventory")
	bool AddVoxels(EVoxelType VoxelType, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category="Voxel Inventory")
	bool RemoveVoxels(EVoxelType VoxelType, int32 Amount = 1);

	UFUNCTION(BlueprintPure, Category="Voxel Inventory")
	int32 GetVoxelCount(EVoxelType VoxelType) const;

	UFUNCTION(BlueprintPure, Category="Voxel Inventory")
	bool HasVoxels(EVoxelType VoxelType, int32 Amount = 1) const;

	UFUNCTION(BlueprintCallable, Category="Voxel Inventory")
	bool SelectVoxelType(EVoxelType VoxelType);

	UFUNCTION(BlueprintCallable, Category="Voxel Inventory")
	bool Increment();

	UFUNCTION(BlueprintCallable, Category="Voxel Inventory")
	bool Decrement();

	UFUNCTION(BlueprintPure, Category="Voxel Inventory")
	EVoxelType GetSelectedVoxelType() const { return SelectedVoxelType; }

	UPROPERTY(BlueprintAssignable, Category="Voxel Inventory")
	FOnVoxelInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category="Voxel Inventory")
	FOnSelectedVoxelTypeChanged OnSelectedVoxelTypeChanged;

private:
	UPROPERTY(Transient)
	TMap<EVoxelType, int32> VoxelCounts;

	UPROPERTY(Transient)
	EVoxelType SelectedVoxelType{ EVoxelType::Grass };
};
