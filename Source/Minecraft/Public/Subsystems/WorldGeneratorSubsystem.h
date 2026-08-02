// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldGeneratorSubsystem.generated.h"

UCLASS()
class MINECRAFT_API UWorldGeneratorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UTexture2D* GenerateHeightmapTexture();

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Debug")
	TObjectPtr<UTexture2D> HeightmapDebugTexture;
};
