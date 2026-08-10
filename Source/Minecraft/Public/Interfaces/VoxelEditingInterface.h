#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Voxels/Data/VoxelData.h"
#include "VoxelEditingInterface.generated.h"

UINTERFACE(BlueprintType)
class MINECRAFT_API UVoxelEditingInterface : public UInterface
{
	GENERATED_BODY()
};

class MINECRAFT_API IVoxelEditingInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Voxel Editing")
	bool AddVoxel(const FVector& WorldLocation, EVoxelType VoxelType);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Voxel Editing")
	bool RemoveVoxel(const FVector& WorldLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Voxel Editing")
	EVoxelType GetVoxelType(const FVector& WorldLocation);
};
