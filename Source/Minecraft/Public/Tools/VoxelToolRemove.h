#pragma once

#include "CoreMinimal.h"
#include "Tools/VoxelToolAction.h"
#include "VoxelToolRemove.generated.h"

UCLASS(Blueprintable, BlueprintType)
class MINECRAFT_API UVoxelToolRemove : public UVoxelToolAction
{
	GENERATED_BODY()

public:
	virtual bool ExecuteTool_Implementation(const FHitResult& HitResult) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Voxel Tool", meta=(ClampMin="0.1"))
	float SurfaceOffset{ 1.0f };
};
