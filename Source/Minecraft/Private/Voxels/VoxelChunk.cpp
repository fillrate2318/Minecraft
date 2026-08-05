// Fill out your copyright notice in the Description page of Project Settings.

#include "Voxels/VoxelChunk.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Voxels/VoxelWorld.h"
#include "Voxels/Data/VoxelData.h"

AVoxelChunk::AVoxelChunk()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	SetRootComponent(SceneRoot);

	SnowISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SnowHISM"));
	SnowISM->SetupAttachment(SceneRoot);
	SnowISM->SetMobility(EComponentMobility::Static);

	GrassISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SnowHISM"));
	GrassISM->SetupAttachment(SceneRoot);
	GrassISM->SetMobility(EComponentMobility::Static);

	RockISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SnowHISM"));
	RockISM->SetupAttachment(SceneRoot);
	RockISM->SetMobility(EComponentMobility::Static);
}

void AVoxelChunk::BeginPlay()
{
	Super::BeginPlay();
}

void AVoxelChunk::BuildInstancedMeshes() const
{
	if (!VoxelWorld)
	{
		return;
	}

	const int32 SurfaceBlocks = Size * Size;
	
	TArray<FTransform> SnowTransforms;
	SnowTransforms.Reserve(SurfaceBlocks);
	
	TArray<FTransform> GrassTransforms;
	GrassTransforms.Reserve(SurfaceBlocks);
	
	TArray<FTransform> RockTransforms;
	RockTransforms.Reserve(SurfaceBlocks);

	for (int32 x = 0; x < Size; ++x)
	{
		for (int32 y = 0; y < Size; ++y)
		{
			for (int32 z = 0; z < Size; ++z)
			{
				const FIntVector Location(x, y, z);
				const EVoxelType VoxelType = Data.GetVoxel(Location);

				// Skip empty voxels
				if (VoxelType == EVoxelType::Empty) continue;

				const FIntVector WorldLocation = LocalToWorldLocation(Location);

				// Skip non-visible voxels
				if (!VoxelWorld->IsVoxelVisible(WorldLocation))
				{
					continue;
				}

				// Assuming mesh pivot is at the center
				const FVector InstanceLocation(static_cast<float>(x) + 0.5f * BlockSize,
					static_cast<float>(y) + 0.5f * BlockSize, static_cast<float>(z) + 0.5f * BlockSize);

				const FTransform InstanceTransform(FRotator::ZeroRotator,
					InstanceLocation, FVector::OneVector);

				switch (VoxelType)
				{
					case EVoxelType::Snow:
						SnowTransforms.Add(InstanceTransform);
						break;
					case EVoxelType::Grass:
						GrassTransforms.Add(InstanceTransform);
						break;
					case EVoxelType::Rock:
						RockTransforms.Add(InstanceTransform);
						break;
					default:
						break;
				}
			}
		}
	}

	SnowISM->AddInstances(SnowTransforms, false, false, false);
	GrassISM->AddInstances(GrassTransforms, false, false, false);
	RockISM->AddInstances(RockTransforms, false, false, false);
}

FIntVector AVoxelChunk::LocalToWorldLocation(const FIntVector& LocalLocation) const
{
	return FIntVector(Coords.X * Size + LocalLocation.X,
		Coords.Y * Size + LocalLocation.Y, LocalLocation.Z);
}

FIntVector AVoxelChunk::WorldToLocalLocation(const FIntVector& WorldLocation) const
{
	return FIntVector(WorldLocation.X % Size, WorldLocation.Y % Size, WorldLocation.Z);
}

void AVoxelChunk::InitializeInstancedStaticMeshComponent(UInstancedStaticMeshComponent* Component)
{
	Component->SetMobility(EComponentMobility::Movable);
	Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Component->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	Component->SetGenerateOverlapEvents(false);
	Component->SetCanEverAffectNavigation(false);
	Component->SetCastShadow(true);
}

EVoxelType AVoxelChunk::GetVoxelFromWorldLocation(const FIntVector& WorldLocation) const
{
	FIntVector LocalLocation = WorldToLocalLocation(WorldLocation);
	return Data.GetVoxel(LocalLocation);
}
