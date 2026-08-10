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

	SnowISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SnowISM"));
	SnowISM->SetupAttachment(SceneRoot);
	InitializeInstancedStaticMeshComponent(SnowISM);

	GrassISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("GrassISM"));
	GrassISM->SetupAttachment(SceneRoot);
	InitializeInstancedStaticMeshComponent(GrassISM);

	RockISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RockISM"));
	RockISM->SetupAttachment(SceneRoot);
	InitializeInstancedStaticMeshComponent(RockISM);
}

void AVoxelChunk::Initialize(AVoxelWorld* InVoxelWorld, const int32 InSize, const int32 InHeight,
	const int32 InBlockSize, const FIntPoint& InCoords)
{
	VoxelWorld = InVoxelWorld;
	Size = InSize;
	Height = InHeight;
	BlockSize = InBlockSize;
	Coords = InCoords;

	AttachToActor(VoxelWorld, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	const FVector RelativeLocation(Coords.X * Size * BlockSize,
		Coords.Y * Size * BlockSize,
		0.0f);
	
	SetActorRelativeLocation(RelativeLocation);
}

void AVoxelChunk::BeginPlay()
{
	Super::BeginPlay();
}

void AVoxelChunk::InitializeRawVoxelData(const FDiamondSquareHeightmap& Heightmap, const FVoxelWorldSettings& Settings)
{
	Data.Init(Size, Height);
	SurfaceHeights.Init(
		INDEX_NONE,
		Size * Size);
	
	for (int32 x = 0; x < Size; ++x)
	{
		for (int32 y = 0; y < Size; ++y)
		{
			const int32 PosX = Coords.X * Size + x;
			const int32 PosY = Coords.Y * Size + y;

			//const float Value = Heightmap.GetValue(PosX, PosY);
			// Smooth
			const float Value =
			(
				Heightmap.GetValue(PosX, PosY) +
				Heightmap.GetValue(PosX + 1, PosY) +
				Heightmap.GetValue(PosX, PosY + 1) +
				Heightmap.GetValue(PosX + 1, PosY + 1)
			) * 0.25f;
			const int32 PosZUnclamped =
				FMath::RoundToInt(FMath::Lerp(static_cast<float>(Settings.MinTerrainHeight),
					static_cast<float>(Settings.MaxTerrainHeight), Value));
			const int32 SurfaceZ = FMath::Clamp(PosZUnclamped, 0, Height - 1);

			SurfaceHeights[x + y * Size] = SurfaceZ;
			
			// Fill column with rock
			for (int32 z = 0; z <= SurfaceZ; ++z)
			{
				Data.SetVoxel(FIntVector(x, y, z), EVoxelType::Rock);
			}
		}
	}
}

void AVoxelChunk::InitializeVoxelTypes(const FVoxelWorldSettings& Settings)
{
	if (!VoxelWorld)
	{
		return;
	}

	for (int32 x = 0; x < Size; ++x)
	{
		for (int32 y = 0; y < Size; ++y)
		{
			const int32 SurfaceZ = SurfaceHeights[x + y * Size];

			if (SurfaceZ == INDEX_NONE)
			{
				continue;
			}

			for (int32 z = 0; z <= SurfaceZ; ++z)
			{
				const FIntVector Pos = FIntVector(x, y, z);

				if (Data.GetVoxel(Pos) == EVoxelType::Empty)
				{
					continue;
				}

				const FIntVector WorldPos = LocalToWorldLocation(Pos);
				const bool bVisible = VoxelWorld->IsVoxelVisible(WorldPos);
				EVoxelType Type = EVoxelType::Rock;

				if (bVisible)
				{
					Type = GetVisibleVoxelType(z, Settings);
				}
				else
				{
					Type = GetInvisibleVoxelType(z, SurfaceZ, Settings);
				}

				Data.SetVoxel(Pos, Type);
			}
		}
	}
}

void AVoxelChunk::InitializeMeshVisualization(UStaticMesh* Mesh, UMaterialInterface* SnowMaterial,
                                              UMaterialInterface* GrassMaterial, UMaterialInterface* RockMaterial) const
{
	SnowISM->SetStaticMesh(Mesh);
	SnowISM->SetMaterial(0, SnowMaterial);

	GrassISM->SetStaticMesh(Mesh);
	GrassISM->SetMaterial(0, GrassMaterial);

	RockISM->SetStaticMesh(Mesh);
	RockISM->SetMaterial(0, RockMaterial);
}

void AVoxelChunk::BuildInstancedMeshes()
{
	if (!VoxelWorld)
	{
		return;
	}

	SnowISM->ClearInstances();
	GrassISM->ClearInstances();
	RockISM->ClearInstances();
	VoxelInstances.Reset();
	SnowInstanceLocations.Reset();
	GrassInstanceLocations.Reset();
	RockInstanceLocations.Reset();

	const int32 SurfaceBlocks = Size * Size;
	
	TArray<FTransform> SnowTransforms;
	SnowTransforms.Reserve(SurfaceBlocks);
	SnowInstanceLocations.Reserve(SurfaceBlocks);
	
	TArray<FTransform> GrassTransforms;
	GrassTransforms.Reserve(SurfaceBlocks);
	GrassInstanceLocations.Reserve(SurfaceBlocks);
	
	TArray<FTransform> RockTransforms;
	RockTransforms.Reserve(SurfaceBlocks);
	RockInstanceLocations.Reserve(SurfaceBlocks);

	for (int32 x = 0; x < Size; ++x)
	{
		for (int32 y = 0; y < Size; ++y)
		{
			for (int32 z = 0; z < Height; ++z)
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
				const FVector InstanceLocation((static_cast<float>(x) + 0.5f) * BlockSize,
					(static_cast<float>(y) + 0.5f) * BlockSize, (static_cast<float>(z) + 0.5f) * BlockSize);
				
				const FTransform InstanceTransform(FRotator::ZeroRotator,
					InstanceLocation, FVector::OneVector);

				switch (VoxelType)
				{
					case EVoxelType::Snow:
						SnowTransforms.Add(InstanceTransform);
						SnowInstanceLocations.Add(Location);
						break;
					case EVoxelType::Grass:
						GrassTransforms.Add(InstanceTransform);
						GrassInstanceLocations.Add(Location);
						break;
					case EVoxelType::Rock:
						RockTransforms.Add(InstanceTransform);
						RockInstanceLocations.Add(Location);
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

	for (int32 Index = 0; Index < SnowInstanceLocations.Num(); ++Index)
	{
		VoxelInstances.Add(SnowInstanceLocations[Index], { EVoxelType::Snow, Index });
	}
	for (int32 Index = 0; Index < GrassInstanceLocations.Num(); ++Index)
	{
		VoxelInstances.Add(GrassInstanceLocations[Index], { EVoxelType::Grass, Index });
	}
	for (int32 Index = 0; Index < RockInstanceLocations.Num(); ++Index)
	{
		VoxelInstances.Add(RockInstanceLocations[Index], { EVoxelType::Rock, Index });
	}
}

void AVoxelChunk::RefreshVoxelInstance(const FIntVector& WorldLocation)
{
	const FIntVector LocalLocation = WorldToLocalLocation(WorldLocation);
	if (!Data.IsValidVoxel(LocalLocation) || !VoxelWorld)
	{
		return;
	}

	const EVoxelType VoxelType = Data.GetVoxel(LocalLocation);
	const bool bShouldHaveInstance = VoxelType != EVoxelType::Empty &&
		VoxelWorld->IsVoxelVisible(WorldLocation);
	const FVoxelInstanceHandle* ExistingHandle = VoxelInstances.Find(LocalLocation);

	if (!bShouldHaveInstance)
	{
		if (ExistingHandle)
		{
			RemoveVoxelInstance(LocalLocation);
		}
		return;
	}

	if (!ExistingHandle)
	{
		AddVoxelInstance(LocalLocation, VoxelType);
	}
	else if (ExistingHandle->Type != VoxelType)
	{
		RemoveVoxelInstance(LocalLocation);
		AddVoxelInstance(LocalLocation, VoxelType);
	}
}

void AVoxelChunk::AddVoxelInstance(const FIntVector& LocalLocation, const EVoxelType VoxelType)
{
	UInstancedStaticMeshComponent* Component = GetInstancedMesh(VoxelType);
	TArray<FIntVector>* InstanceLocations = GetInstanceLocations(VoxelType);
	if (!Component || !InstanceLocations)
	{
		return;
	}

	const FVector InstanceLocation(
		(static_cast<float>(LocalLocation.X) + 0.5f) * BlockSize,
		(static_cast<float>(LocalLocation.Y) + 0.5f) * BlockSize,
		(static_cast<float>(LocalLocation.Z) + 0.5f) * BlockSize);
	const int32 InstanceIndex = Component->AddInstance(
		FTransform(FRotator::ZeroRotator, InstanceLocation, FVector::OneVector));

	if (InstanceIndex != INDEX_NONE)
	{
		InstanceLocations->Add(LocalLocation);
		VoxelInstances.Add(LocalLocation, { VoxelType, InstanceIndex });
	}
}

void AVoxelChunk::RemoveVoxelInstance(const FIntVector& LocalLocation)
{
	const FVoxelInstanceHandle* FoundHandle = VoxelInstances.Find(LocalLocation);
	if (!FoundHandle)
	{
		return;
	}

	const FVoxelInstanceHandle Handle = *FoundHandle;
	UInstancedStaticMeshComponent* Component = GetInstancedMesh(Handle.Type);
	TArray<FIntVector>* InstanceLocations = GetInstanceLocations(Handle.Type);
	if (!Component || !InstanceLocations || !InstanceLocations->IsValidIndex(Handle.Index))
	{
		return;
	}

	const int32 LastIndex = InstanceLocations->Num() - 1;
	const FIntVector SwappedLocation = (*InstanceLocations)[LastIndex];
	if (!Component->RemoveInstance(Handle.Index))
	{
		return;
	}

	InstanceLocations->RemoveAtSwap(Handle.Index, 1, EAllowShrinking::No);
	VoxelInstances.Remove(LocalLocation);

	if (Handle.Index != LastIndex)
	{
		if (FVoxelInstanceHandle* SwappedHandle = VoxelInstances.Find(SwappedLocation))
		{
			SwappedHandle->Index = Handle.Index;
		}
	}
}

UInstancedStaticMeshComponent* AVoxelChunk::GetInstancedMesh(const EVoxelType VoxelType) const
{
	switch (VoxelType)
	{
		case EVoxelType::Snow: return SnowISM;
		case EVoxelType::Grass: return GrassISM;
		case EVoxelType::Rock: return RockISM;
		default: return nullptr;
	}
}

TArray<FIntVector>* AVoxelChunk::GetInstanceLocations(const EVoxelType VoxelType)
{
	switch (VoxelType)
	{
		case EVoxelType::Snow: return &SnowInstanceLocations;
		case EVoxelType::Grass: return &GrassInstanceLocations;
		case EVoxelType::Rock: return &RockInstanceLocations;
		default: return nullptr;
	}
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
	Component->SetRemoveSwap();
	Component->SetMobility(EComponentMobility::Movable);
	Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Component->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	Component->SetGenerateOverlapEvents(false);
	Component->SetCanEverAffectNavigation(false);
	Component->SetCastShadow(true);
}

EVoxelType AVoxelChunk::GetVisibleVoxelType(const int32 Z, const FVoxelWorldSettings& Settings) const
{
	int32 SnowBorder = FMath::RandRange(Settings.SnowBorderLowerLimit, Settings.SnowBorderUpperLimit);
	if (Z >= SnowBorder)
	{
		return EVoxelType::Snow;
	}

	int32 GrassBorder = FMath::RandRange(Settings.GrassBorderLowerLimit, Settings.GrassBorderUpperLimit);
	if (Z >= GrassBorder)
	{
		return EVoxelType::Grass;
	}
	
	return EVoxelType::Rock;
}

EVoxelType AVoxelChunk::GetInvisibleVoxelType(const int32 Z,
	const int32 SurfaceZ, const FVoxelWorldSettings& Settings) const
{
	const int32 Depth = SurfaceZ - Z;

	if (Depth > Settings.RockDepth)
	{
		return EVoxelType::Rock;
	}
	
	const EVoxelType SurfaceType = GetVisibleVoxelType(Z, Settings);
	return SurfaceType;
}

EVoxelType AVoxelChunk::GetVoxelFromWorldLocation(const FIntVector& WorldLocation) const
{
	FIntVector LocalLocation = WorldToLocalLocation(WorldLocation);
	return Data.GetVoxel(LocalLocation);
}

bool AVoxelChunk::SetVoxelFromWorldLocation(const FIntVector& WorldLocation, const EVoxelType VoxelType)
{
	const FIntVector LocalLocation = WorldToLocalLocation(WorldLocation);
	if (!Data.IsValidVoxel(LocalLocation) || Data.GetVoxel(LocalLocation) == VoxelType)
	{
		return false;
	}

	return Data.SetVoxel(LocalLocation, VoxelType);
}
