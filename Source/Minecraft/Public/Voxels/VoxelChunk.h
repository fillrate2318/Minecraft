// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/VoxelData.h"
#include "GameFramework/Actor.h"
#include "Heightmap/DiamondSquareGenerator.h"
#include "VoxelChunk.generated.h"

class AVoxelWorld;

UCLASS()
class MINECRAFT_API AVoxelChunk : public AActor
{
	GENERATED_BODY()

public:
	AVoxelChunk();

	void Initialize(AVoxelWorld* InVoxelWorld, const int32 InSize, const int32 InHeight,
		const int32 InBlockSize, const FIntPoint& InCoords);
	void InitializeRawVoxelData(const FDiamondSquareHeightmap& Heightmap, const FVoxelWorldSettings& Settings);
	void InitializeVoxelTypes(const FVoxelWorldSettings& Settings);
	void InitializeMeshVisualization(UStaticMesh* Mesh, UMaterialInterface* SnowMaterial,
		UMaterialInterface* GrassMaterial, UMaterialInterface* RockMaterial) const;
	void BuildInstancedMeshes() const;
	
	EVoxelType GetVoxelFromWorldLocation(const FIntVector& WorldLocation) const;
	
protected:
	virtual void BeginPlay() override;
	
	FIntVector LocalToWorldLocation(const FIntVector& LocalLocation) const;
	FIntVector WorldToLocalLocation(const FIntVector& WorldLocation) const;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInstancedStaticMeshComponent> SnowISM;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInstancedStaticMeshComponent> GrassISM;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInstancedStaticMeshComponent> RockISM;

	UPROPERTY()
	TObjectPtr<AVoxelWorld> VoxelWorld;

private:
	void InitializeInstancedStaticMeshComponent(UInstancedStaticMeshComponent* Component);
	EVoxelType GetVisibleVoxelType(const int32 Z, const FVoxelWorldSettings& Settings) const;
	EVoxelType GetInvisibleVoxelType(const int32 Z, const int32 SurfaceZ,
		const FVoxelWorldSettings& Settings) const;

	
	FVoxelChunkData Data;
	FIntPoint Coords;
	
	
	int32 Size = 32;
	int32 Height = 128;

	float BlockSize = 100.0f;

	TArray<int32> SurfaceHeights;
};
