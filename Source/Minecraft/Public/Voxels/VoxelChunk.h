// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/VoxelData.h"
#include "GameFramework/Actor.h"
#include "VoxelChunk.generated.h"

class AVoxelWorld;

UCLASS()
class MINECRAFT_API AVoxelChunk : public AActor
{
	GENERATED_BODY()

public:
	AVoxelChunk();
	
	EVoxelType GetVoxelFromWorldLocation(const FIntVector& WorldLocation) const;
	
protected:
	virtual void BeginPlay() override;

	void BuildInstancedMeshes() const;
	
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
	
	FVoxelChunkData Data;
	FIntPoint Coords;
	
	
	int32 Size = 32;
	int32 Height = 128;

	float BlockSize = 100.0f;
};
