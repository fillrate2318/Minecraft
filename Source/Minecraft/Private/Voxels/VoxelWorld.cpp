// Fill out your copyright notice in the Description page of Project Settings.

#include "Voxels/VoxelWorld.h"

#include "DeveloperSettings/VoxelDeveloperSettings.h"
#include "Subsystems/WorldGeneratorSubsystem.h"
#include "Voxels/VoxelChunk.h"

AVoxelWorld::AVoxelWorld()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVoxelWorld::BeginPlay()
{
	Super::BeginPlay();

	ConstructFromHeightmap();
}

void AVoxelWorld::ConstructFromHeightmap()
{
	UWorldGeneratorSubsystem* WGS = UWorldGeneratorSubsystem::Get(this->GetWorld()); 
	
	if (!WGS) return;

	FDiamondSquareHeightmap& Heightmap = WGS->GetHeightmap();

	if (!Heightmap.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Generated heightmap is invalid"));
		return;
	}
	
	WorldSize = Heightmap.Size - 1;

	const int32 ChunkCount = WorldSize / ChunkSize;

	const UVoxelDeveloperSettings* Settings = UVoxelDeveloperSettings::Get();
	if (!Settings)
	{
		UE_LOG(LogTemp, Error, TEXT("Voxel developer settings not found"));
		return;
	}

	UStaticMesh* Mesh = Settings->BlockMesh.LoadSynchronous();
	UMaterialInterface* SnowMaterial = Settings->SnowMaterialInstance.LoadSynchronous();
	UMaterialInterface* GrassMaterial = Settings->GrassMaterialInstance.LoadSynchronous();
	UMaterialInterface* RockMaterial = Settings->RockMaterialInstance.LoadSynchronous();
	
	// First spawn chunks and fill voxel data
	for (int32 x = 0; x < ChunkCount; ++x)
	{
		for (int32 y = 0; y < ChunkCount; ++y)
		{
			const FIntPoint Position = FIntPoint(x, y);
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AVoxelChunk* Chunk = GetWorld()->SpawnActor<AVoxelChunk>(ChunkClass, GetActorTransform(), SpawnParameters);

			if (!Chunk)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn voxel chunk"));
				continue;
			}

			Chunk->Initialize(this, ChunkSize, ChunkHeight, BlockSize, Position);
			Chunk->InitializeRawVoxelData(Heightmap, WorldSettings);
			Chunk->InitializeMeshVisualization(Mesh, SnowMaterial, GrassMaterial, RockMaterial);
			Chunks.Add(Position, Chunk);
		}
	}

	for (auto& Pair : Chunks)
	{
		Pair.Value->InitializeVoxelTypes(WorldSettings);
	}
	
	// Iterate over spawned chunks and fill ISMs 
	for (auto& Pair : Chunks)
	{
		Pair.Value->BuildInstancedMeshes();
	}
}

int32 AVoxelWorld::ConvertHeightToVoxelZ(const float Height, int32 MinHeight, int32 MaxHeight)
{
	// Lerp clamped height between min and max values and round result
	return FMath::RoundToInt(FMath::Lerp(static_cast<float>(MinHeight), static_cast<float>(MaxHeight),
		FMath::Clamp(Height, 0.f, 1.f)));
}

bool AVoxelWorld::IsVoxelVisible(const FIntVector& VoxelWorldLocation)
{
	// Check whether any face of a voxel is in contact with empty space
	for (const FIntVector& Direction : Directions)
	{
		const FIntVector Position = VoxelWorldLocation + Direction;

		if (Position.Z < 0)
		{
			continue;
		}

		if (GetVoxel(Position) == EVoxelType::Empty)
		{
			return true;
		}
	}
	return false;
}

EVoxelType AVoxelWorld::GetVoxel(const FIntVector& VoxelWorldLocation)
{
	FIntPoint Coords(VoxelWorldLocation.X / ChunkSize, VoxelWorldLocation.Y / ChunkSize);
	AVoxelChunk* Chunk = GetVoxelChunk(Coords);
	
	if (!Chunk)
	{
		return EVoxelType::Empty;
	}

	return Chunk->GetVoxelFromWorldLocation(VoxelWorldLocation);
}

AVoxelChunk* AVoxelWorld::GetVoxelChunk(const FIntPoint& Coords)
{
	const TObjectPtr<AVoxelChunk>* Found = Chunks.Find(Coords);
	return Found ? Found->Get() : nullptr;
}
