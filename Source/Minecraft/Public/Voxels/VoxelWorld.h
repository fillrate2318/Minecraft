// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/VoxelData.h"
#include "GameFramework/Actor.h"
#include "VoxelWorld.generated.h"

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

UCLASS()
class MINECRAFT_API AVoxelWorld : public AActor
{
	GENERATED_BODY()

public:
	AVoxelWorld();

	bool IsVoxelVisible(const FIntVector& VoxelWorldLocation);
	
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
	static int32 ConvertHeightToVoxelZ(const float HeightAlpha, int32 MinHeight, int32 MaxHeight);

	EVoxelType GetVoxel(const FIntVector& VoxelWorldLocation);
	AVoxelChunk* GetVoxelChunk(const FIntPoint& Coords);

	int32 WorldSize;
	
	UPROPERTY()
	TMap<FIntPoint, TObjectPtr<AVoxelChunk>> Chunks;
	
};
