// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/VoxelData.h"
#include "GameFramework/Actor.h"
#include "VoxelWorld.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVoxelWorldReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnVoxelChanged, const FIntVector&, VoxelLocation, EVoxelType, VoxelType);

static const FIntVector Directions[] =
	{
	FIntVector( 1,  0,  0),
	FIntVector(-1,  0,  0),
	FIntVector( 0,  1,  0),
	FIntVector( 0, -1,  0),
	FIntVector( 0,  0,  1),
	FIntVector( 0,  0, -1)
};

class AVoxelChunk;
class UBoxComponent;
class USceneComponent;

UCLASS()
class MINECRAFT_API AVoxelWorld : public AActor
{
	GENERATED_BODY()

public:
	AVoxelWorld();

	bool IsVoxelVisible(const FIntVector& VoxelWorldLocation) const;
	bool IsWorldReady() const { return bWorldReady; }
	bool TryGetSpawnTransform(float VerticalClearance, FTransform& OutTransform) const;

	UFUNCTION(BlueprintPure, Category="Voxel World|Editing")
	bool WorldLocationToVoxel(const FVector& WorldLocation, FIntVector& OutVoxelLocation) const;

	UFUNCTION(BlueprintCallable, Category="Voxel World|Editing")
	bool RemoveVoxelAtWorldLocation(const FVector& WorldLocation);

	UFUNCTION(BlueprintCallable, Category="Voxel World|Editing")
	bool AddVoxelAtWorldLocation(const FVector& WorldLocation, EVoxelType VoxelType);

	UFUNCTION(BlueprintPure, Category="Voxel World|Editing")
	EVoxelType GetVoxelAtWorldLocation(const FVector& WorldLocation) const;

	UPROPERTY(BlueprintAssignable, Category="Voxel World|Callbacks")
	FOnVoxelWorldReady OnWorldReady;

	UPROPERTY(BlueprintAssignable, Category="Voxel World|Callbacks")
	FOnVoxelChanged OnVoxelChanged;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Voxel World");
	int32 BlockSize{ 100 };
	
	UPROPERTY(EditAnywhere, Category="Voxel World");
	int32 ChunkSize{ 32 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel World")
	int32 ChunkHeight{ 128 };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel World")
	FVoxelWorldSettings WorldSettings;
	
	UPROPERTY(EditAnywhere, Category="Voxel World")
	TSubclassOf<AVoxelChunk> ChunkClass;

private:
	void ConstructFromHeightmap();
	void ConfigureBoundaryCollision();
	void MarkWorldReady();
	static int32 ConvertHeightToVoxelZ(const float HeightAlpha, int32 MinHeight, int32 MaxHeight);

	EVoxelType GetVoxel(const FIntVector& VoxelWorldLocation) const;
	AVoxelChunk* GetVoxelChunk(const FIntPoint& Coords) const;
	bool SetVoxel(const FIntVector& VoxelWorldLocation, EVoxelType VoxelType);

	int32 WorldSize{ 0 };
	bool bWorldReady{ false };

	UPROPERTY(VisibleAnywhere, Category="Voxel World|Collision")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category="Voxel World|Collision")
	TArray<TObjectPtr<UBoxComponent>> BoundaryWalls;
	
	UPROPERTY()
	TMap<FIntPoint, TObjectPtr<AVoxelChunk>> Chunks;
	
};
