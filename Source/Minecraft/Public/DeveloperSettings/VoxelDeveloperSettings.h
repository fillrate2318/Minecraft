// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "VoxelDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class MINECRAFT_API UVoxelDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const UVoxelDeveloperSettings* Get() { return GetDefault<UVoxelDeveloperSettings>(); }

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Voxel Developer Settings")
	TSoftObjectPtr<UMaterialInterface> SnowMaterialInstance;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Voxel Developer Settings")
	TSoftObjectPtr<UMaterialInterface> GrassMaterialInstance;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Voxel Developer Settings")
	TSoftObjectPtr<UMaterialInterface> RockMaterialInstance;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Voxel Developer Settings")
	TSoftObjectPtr<UStaticMesh> BlockMesh;
};
