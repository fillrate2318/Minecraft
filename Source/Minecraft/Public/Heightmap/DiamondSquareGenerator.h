// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FDiamondSquareSettings
{
	int32 SampleSize{ 1024 };

	FDiamondSquareSettings(const int32 SampleSize) : SampleSize(SampleSize) {}
};

struct FDiamondSquareHeightmap
{
	FDiamondSquareHeightmap(const FDiamondSquareSettings& Settings)
	{
		Values.Init(0.f, Settings.SampleSize * Settings.SampleSize);
	}
	
	TArray<float> Values;
	
	void SetValue(const int32 X, const int32 Y, const float Value)
	{
		Values[GetIndex(X, Y)] = Value;
	}

	float GetValue(const int32 X, const int32 Y) const
	{
		return Values[GetIndex(X, Y)];
	}

private:
	static int32 GetIndex(const int32 X, const int32 Y)
	{
		return X + Y * 1024;
	}
};

class UDiamondSquareGenerator
{
public:
	static void Generate(const FDiamondSquareSettings& Settings, FDiamondSquareHeightmap& OutHeightmap);
	static void Normalize(FDiamondSquareHeightmap& Heightmap);
};
