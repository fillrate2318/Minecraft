// Fill out your copyright notice in the Description page of Project Settings.

#include "Voxels/VoxelWorld.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "DeveloperSettings/VoxelDeveloperSettings.h"
#include "Subsystems/WorldGeneratorSubsystem.h"
#include "Voxels/VoxelChunk.h"
#include "TimerManager.h"

AVoxelWorld::AVoxelWorld()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	static const FName WallNames[] =
	{
		TEXT("MinXBoundary"),
		TEXT("MaxXBoundary"),
		TEXT("MinYBoundary"),
		TEXT("MaxYBoundary")
	};

	for (const FName WallName : WallNames)
	{
		UBoxComponent* BoundaryWall = CreateDefaultSubobject<UBoxComponent>(WallName);
		BoundaryWall->SetupAttachment(SceneRoot);
		BoundaryWall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BoundaryWall->SetCollisionResponseToAllChannels(ECR_Ignore);
		BoundaryWall->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		BoundaryWall->SetGenerateOverlapEvents(false);
		BoundaryWalls.Add(BoundaryWall);
	}
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
	ConfigureBoundaryCollision();

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

	if (!Chunks.IsEmpty())
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AVoxelWorld::MarkWorldReady);
	}
}

void AVoxelWorld::ConfigureBoundaryCollision()
{
	if (BoundaryWalls.Num() != 4 || WorldSize <= 0 || BlockSize <= 0 || ChunkHeight <= 0)
	{
		return;
	}

	const float WorldExtent = static_cast<float>(WorldSize * BlockSize);
	const float WorldHalfExtent = WorldExtent * 0.5f;
	const float WallThickness = static_cast<float>(BlockSize);
	const float WallHalfThickness = WallThickness * 0.5f;
	const float WallHalfHeight = static_cast<float>(ChunkHeight * BlockSize) * 0.5f;

	BoundaryWalls[0]->SetBoxExtent(FVector(WallHalfThickness, WorldHalfExtent, WallHalfHeight));
	BoundaryWalls[0]->SetRelativeLocation(FVector(-WallHalfThickness, WorldHalfExtent, WallHalfHeight));

	BoundaryWalls[1]->SetBoxExtent(FVector(WallHalfThickness, WorldHalfExtent, WallHalfHeight));
	BoundaryWalls[1]->SetRelativeLocation(FVector(WorldExtent + WallHalfThickness, WorldHalfExtent, WallHalfHeight));

	BoundaryWalls[2]->SetBoxExtent(FVector(WorldHalfExtent, WallHalfThickness, WallHalfHeight));
	BoundaryWalls[2]->SetRelativeLocation(FVector(WorldHalfExtent, -WallHalfThickness, WallHalfHeight));

	BoundaryWalls[3]->SetBoxExtent(FVector(WorldHalfExtent, WallHalfThickness, WallHalfHeight));
	BoundaryWalls[3]->SetRelativeLocation(FVector(WorldHalfExtent, WorldExtent + WallHalfThickness, WallHalfHeight));
}

void AVoxelWorld::MarkWorldReady()
{
	bWorldReady = true;
	OnWorldReady.Broadcast();
}

bool AVoxelWorld::TryGetSpawnTransform(const float VerticalClearance, FTransform& OutTransform) const
{
	if (!bWorldReady || WorldSize <= 0)
	{
		return false;
	}

	const UWorldGeneratorSubsystem* WGS = UWorldGeneratorSubsystem::Get(GetWorld());
	if (!WGS)
	{
		return false;
	}

	const FDiamondSquareHeightmap& Heightmap = WGS->GetHeightmap();
	const int32 VoxelX = FMath::Clamp(WorldSize / 2, 0, WorldSize - 1);
	const int32 VoxelY = FMath::Clamp(WorldSize / 2, 0, WorldSize - 1);
	const float HeightAlpha =
		(Heightmap.GetValue(VoxelX, VoxelY) +
		 Heightmap.GetValue(VoxelX + 1, VoxelY) +
		 Heightmap.GetValue(VoxelX, VoxelY + 1) +
		 Heightmap.GetValue(VoxelX + 1, VoxelY + 1)) * 0.25f;
	const int32 SurfaceZ = FMath::Clamp(
		ConvertHeightToVoxelZ(HeightAlpha, WorldSettings.MinTerrainHeight, WorldSettings.MaxTerrainHeight),
		0, ChunkHeight - 1);
	const FVector LocalLocation(
		(static_cast<float>(VoxelX) + 0.5f) * BlockSize,
		(static_cast<float>(VoxelY) + 0.5f) * BlockSize,
		(static_cast<float>(SurfaceZ) + 1.0f) * BlockSize + VerticalClearance);

	OutTransform = FTransform(GetActorRotation(), GetActorTransform().TransformPosition(LocalLocation));
	return true;
}

bool AVoxelWorld::WorldLocationToVoxel(const FVector& WorldLocation, FIntVector& OutVoxelLocation) const
{
	if (!bWorldReady || BlockSize <= 0)
	{
		return false;
	}

	const FVector LocalLocation = GetActorTransform().InverseTransformPosition(WorldLocation);
	OutVoxelLocation = FIntVector(
		FMath::FloorToInt(LocalLocation.X / BlockSize),
		FMath::FloorToInt(LocalLocation.Y / BlockSize),
		FMath::FloorToInt(LocalLocation.Z / BlockSize));

	if (OutVoxelLocation.X < 0 || OutVoxelLocation.Y < 0 || OutVoxelLocation.Z < 0 ||
		OutVoxelLocation.Z >= ChunkHeight)
	{
		return false;
	}

	return GetVoxelChunk(
		FIntPoint(OutVoxelLocation.X / ChunkSize, OutVoxelLocation.Y / ChunkSize)) != nullptr;
}

bool AVoxelWorld::RemoveVoxelAtWorldLocation(const FVector& WorldLocation)
{
	FIntVector VoxelLocation;
	return WorldLocationToVoxel(WorldLocation, VoxelLocation) &&
		VoxelLocation.Z > 0 &&
		SetVoxel(VoxelLocation, EVoxelType::Empty);
}

bool AVoxelWorld::AddVoxelAtWorldLocation(const FVector& WorldLocation, const EVoxelType VoxelType)
{
	if (VoxelType == EVoxelType::Empty)
	{
		return false;
	}

	FIntVector VoxelLocation;
	return WorldLocationToVoxel(WorldLocation, VoxelLocation) && SetVoxel(VoxelLocation, VoxelType);
}

EVoxelType AVoxelWorld::GetVoxelAtWorldLocation(const FVector& WorldLocation) const
{
	FIntVector VoxelLocation;
	return WorldLocationToVoxel(WorldLocation, VoxelLocation)
		? GetVoxel(VoxelLocation)
		: EVoxelType::Empty;
}

int32 AVoxelWorld::ConvertHeightToVoxelZ(const float Height, int32 MinHeight, int32 MaxHeight)
{
	// Lerp clamped height between min and max values and round result
	return FMath::RoundToInt(FMath::Lerp(static_cast<float>(MinHeight), static_cast<float>(MaxHeight),
		FMath::Clamp(Height, 0.f, 1.f)));
}

bool AVoxelWorld::IsVoxelVisible(const FIntVector& VoxelWorldLocation) const
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

EVoxelType AVoxelWorld::GetVoxel(const FIntVector& VoxelWorldLocation) const
{
	FIntPoint Coords(VoxelWorldLocation.X / ChunkSize, VoxelWorldLocation.Y / ChunkSize);
	AVoxelChunk* Chunk = GetVoxelChunk(Coords);
	
	if (!Chunk)
	{
		return EVoxelType::Empty;
	}

	return Chunk->GetVoxelFromWorldLocation(VoxelWorldLocation);
}

AVoxelChunk* AVoxelWorld::GetVoxelChunk(const FIntPoint& Coords) const
{
	const TObjectPtr<AVoxelChunk>* Found = Chunks.Find(Coords);
	return Found ? Found->Get() : nullptr;
}

bool AVoxelWorld::SetVoxel(const FIntVector& VoxelWorldLocation, const EVoxelType VoxelType)
{
	if (!bWorldReady || VoxelWorldLocation.X < 0 || VoxelWorldLocation.Y < 0 ||
		VoxelWorldLocation.Z < 0 || VoxelWorldLocation.Z >= ChunkHeight)
	{
		return false;
	}

	AVoxelChunk* EditedChunk = GetVoxelChunk(
		FIntPoint(VoxelWorldLocation.X / ChunkSize, VoxelWorldLocation.Y / ChunkSize));
	if (!EditedChunk || !EditedChunk->SetVoxelFromWorldLocation(VoxelWorldLocation, VoxelType))
	{
		return false;
	}

	EditedChunk->RefreshVoxelInstance(VoxelWorldLocation);
	for (const FIntVector& Direction : Directions)
	{
		const FIntVector NeighborLocation = VoxelWorldLocation + Direction;
		if (NeighborLocation.X < 0 || NeighborLocation.Y < 0 ||
			NeighborLocation.Z < 0 || NeighborLocation.Z >= ChunkHeight)
		{
			continue;
		}

		if (AVoxelChunk* NeighborChunk = GetVoxelChunk(
			FIntPoint(NeighborLocation.X / ChunkSize, NeighborLocation.Y / ChunkSize)))
		{
			NeighborChunk->RefreshVoxelInstance(NeighborLocation);
		}
	}

	OnVoxelChanged.Broadcast(VoxelWorldLocation, VoxelType);
	return true;
}
