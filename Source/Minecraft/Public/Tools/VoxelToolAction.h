#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VoxelToolAction.generated.h"

class AActor;
class UVoxelToolComponent;
class UVoxelInventorySubsystem;

UCLASS(Abstract, Blueprintable, BlueprintType)
class MINECRAFT_API UVoxelToolAction : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintNativeEvent, Category="Voxel Tool")
	bool ExecuteTool(const FHitResult& HitResult);
	virtual bool ExecuteTool_Implementation(const FHitResult& HitResult);

	UFUNCTION(BlueprintPure, Category="Voxel Tool")
	UVoxelToolComponent* GetToolComponent() const { return ToolComponent; }

	UFUNCTION(BlueprintPure, Category="Voxel Tool")
	AActor* GetToolOwner() const;

	UFUNCTION(BlueprintPure, Category="Voxel Tool")
	UVoxelInventorySubsystem* GetInventory() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Voxel Tool", meta=(DisplayName="Tool Initialized"))
	void ReceiveToolInitialized();

private:
	friend class UVoxelToolComponent;
	void Initialize(UVoxelToolComponent* InToolComponent);

	UPROPERTY(Transient)
	TObjectPtr<UVoxelToolComponent> ToolComponent;
};
