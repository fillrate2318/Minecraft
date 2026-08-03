// Fill out your copyright notice in the Description page of Project Settings.

#include "Voxels/VoxelChunk.h"

AVoxelChunk::AVoxelChunk()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVoxelChunk::BeginPlay()
{
	Super::BeginPlay();
}
