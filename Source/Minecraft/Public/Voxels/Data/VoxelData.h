// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelData.generated.h"

UENUM(BlueprintType)
enum class EVoxelType : uint8
{
	Empty,
	Snow,
	Grass,
	Rock
};

USTRUCT(BlueprintType)
struct FVoxelChunkData
{
	GENERATED_BODY()

	void Init(const int32 InSize, const int32 InHeight)
	{
		Size = InSize;
		Height = InHeight;

		// Initialize voxels array
		Voxels.Init(EVoxelType::Empty, Size * Size * Height);
	}

	bool IsValidVoxel(const FIntVector& Location) const
	{
		return Location.X >= 0 && Location.X < Size &&
			Location.Y >= 0 && Location.Y < Size &&
			Location.Z >= 0 && Location.Z < Height;
	}
	
	int32 GetIndex(const FIntVector& Location) const
	{
		return Location.X + Location.Y * Size + Location.Z * Size * Size;
	}

	EVoxelType GetVoxel(const FIntVector& Location) const
	{
		if (!IsValidVoxel(Location))
		{
			return EVoxelType::Empty;
		}
		return Voxels[GetIndex(Location)];
	}

	bool SetVoxel(const FIntVector& Location, const EVoxelType Voxel)
	{
		if (!IsValidVoxel(Location))
		{
			return false;
		}

		Voxels[GetIndex(Location)] = Voxel;
		return true;
	}

private:
	int32 Size{ 0 };
	int32 Height{ 0 };

	TArray<EVoxelType> Voxels;
};

USTRUCT(BlueprintType)
struct FVoxelWorldSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 MinTerrainHeight{ 8 };
	
	UPROPERTY(EditAnywhere)
	int32 MaxTerrainHeight{ 80 };

	UPROPERTY(EditAnywhere)
	int32 SnowBorderUpperLimit = 65;

	UPROPERTY(EditAnywhere)
	int32 SnowBorderLowerLimit = 60;
	
	UPROPERTY(EditAnywhere)
	int32 GrassBorderUpperLimit = 30;

	UPROPERTY(EditAnywhere)
	int32 GrassBorderLowerLimit = 25;
	
	UPROPERTY(EditAnywhere)
	int32 RockDepth = 3;
	
};
