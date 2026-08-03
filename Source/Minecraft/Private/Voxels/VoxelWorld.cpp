// Fill out your copyright notice in the Description page of Project Settings.

#include "Voxels/VoxelWorld.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Subsystems/WorldGeneratorSubsystem.h"

AVoxelWorld::AVoxelWorld()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	SetRootComponent(SceneRoot);

	ISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StoneHISM"));
	ISM->SetupAttachment(SceneRoot);
	ISM->SetMobility(EComponentMobility::Static);
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
	VoxelData.SetNum(Size * Size);

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
	}

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
	ISM->AddInstances(VoxelData, false);
}
