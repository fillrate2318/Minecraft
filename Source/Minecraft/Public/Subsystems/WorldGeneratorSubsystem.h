// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Heightmap/DiamondSquareGenerator.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldGeneratorSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHeightmapGenerationFinished);

UCLASS()
class MINECRAFT_API UWorldGeneratorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UWorldGeneratorSubsystem* Get(UWorld* WorldContext);

	FDiamondSquareHeightmap& GetHeightmap() { return Heightmap; }
	const FDiamondSquareHeightmap& GetHeightmap() const { return Heightmap; }
	
	UFUNCTION(BlueprintCallable, Category = "Heightmap")
	void GenerateHeightmap(const FDiamondSquareSettings& InSettings);

	UFUNCTION(BlueprintCallable, Category = "Heightmap|Utils")
	UTexture2D* GetHeightmapAsTexture();

	UPROPERTY(BlueprintAssignable, Category="Callbacks")
	FOnHeightmapGenerationFinished OnHeightmapGenerationFinished;
	
private:
	FDiamondSquareHeightmap Heightmap;

	UPROPERTY()
	TObjectPtr<UTexture2D> HeightmapTexture;
};
