// Fill out your copyright notice in the Description page of Project Settings.

#include "Voxels/VoxelWorld.h"
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
	
	constexpr int32 MinHeight = 8;
	constexpr int32 MaxHeight = 80;

	
	int32 Size = Heightmap.Size;
	/*VoxelData.SetNum(Size * Size);

	for (int32 x = 0; x < Size; ++x)
	{
		for (int32 y = 0; y < Size; ++y)
		{
			int32 XCoord = BlockSize * x;
			int32 YCoord = BlockSize * y;
			int32 ZCoord =
				BlockSize * ConvertHeightToVoxelZ(Heightmap.GetValue(x, y), MinHeight, MaxHeight);
			VoxelData[x + Size * y] = FTransform(FRotator::ZeroRotator, FVector(XCoord, YCoord, ZCoord));
		}
	}*/

	BuildInstances();
}

int32 AVoxelWorld::ConvertHeightToVoxelZ(const float Height, int32 MinHeight, int32 MaxHeight)
{
	// Lerp clamped height between min and max values and round result
	return FMath::RoundToInt(FMath::Lerp(static_cast<float>(MinHeight), static_cast<float>(MaxHeight),
		FMath::Clamp(Height, 0.f, 1.f)));
}

void AVoxelWorld::BuildInstances()
{
	//ISM->AddInstances(VoxelData, false);
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
