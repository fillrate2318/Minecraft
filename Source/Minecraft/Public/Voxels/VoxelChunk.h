// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelChunk.generated.h"

UCLASS()
class MINECRAFT_API AVoxelChunk : public AActor
{
	GENERATED_BODY()

public:
	AVoxelChunk();

protected:
	virtual void BeginPlay() override;

};
