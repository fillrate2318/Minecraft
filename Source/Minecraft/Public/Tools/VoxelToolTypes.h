#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "VoxelToolTypes.generated.h"

class UVoxelToolAction;

USTRUCT(BlueprintType)
struct MINECRAFT_API FVoxelToolConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tool")
	FText ToolName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tool")
	TObjectPtr<UTexture2D> ToolImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tool")
	TSubclassOf<UVoxelToolAction> ToolObjectClass;
};
