#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tools/VoxelToolTypes.h"
#include "VoxelToolComponent.generated.h"

class UVoxelToolAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnActiveVoxelToolChanged, int32, ToolIndex, const FVoxelToolConfig&, ToolConfig);

UCLASS(ClassGroup=(Voxel), meta=(BlueprintSpawnableComponent))
class MINECRAFT_API UVoxelToolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVoxelToolComponent();

	UFUNCTION(BlueprintCallable, Category="Voxel Tool")
	bool ActivateTool(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category="Voxel Tool")
	bool SelectNextTool();

	UFUNCTION(BlueprintCallable, Category="Voxel Tool")
	bool SelectPreviousTool();

	UFUNCTION(BlueprintCallable, Category="Voxel Tool")
	bool SetActiveToolIndex(int32 NewToolIndex);

	UFUNCTION(BlueprintPure, Category="Voxel Tool")
	int32 GetActiveToolIndex() const { return ActiveToolIndex; }

	UFUNCTION(BlueprintPure, Category="Voxel Tool")
	UVoxelToolAction* GetActiveTool() const;

	UFUNCTION(BlueprintPure, Category="Voxel Tool")
	bool GetActiveToolConfig(FVoxelToolConfig& OutToolConfig) const;

	UPROPERTY(BlueprintAssignable, Category="Voxel Tool")
	FOnActiveVoxelToolChanged OnActiveToolChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Voxel Tool")
	TArray<FVoxelToolConfig> ToolConfigurations;

private:
	void CreateTools();
	int32 FindNextValidToolIndex(int32 StartIndex, int32 Direction) const;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UVoxelToolAction>> ToolObjects;

	UPROPERTY(Transient)
	int32 ActiveToolIndex{ INDEX_NONE };
};
