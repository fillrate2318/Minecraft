// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelWorld.generated.h"

UCLASS()
class MINECRAFT_API AVoxelWorld : public AActor
{
	GENERATED_BODY()

public:
	AVoxelWorld();

	UPROPERTY(EditAnywhere, Category=("Voxel World"));
	int32 BlockSize{ 100 };
	
	UPROPERTY(EditAnywhere, Category=("Voxel World"));
	int32 ChunkSize{ 32 };
	
protected:
	virtual void BeginPlay() override;

	
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInstancedStaticMeshComponent> ISM;
	
private:
	void ConstructFromHeightmap();
	static int32 ConvertHeightToVoxelZ(const float HeightAlpha, int32 MinHeight, int32 MaxHeight);
	void BuildInstances();
	
	//TArray<FIntVector> VoxelData;
	TArray<FTransform> VoxelData;
};
