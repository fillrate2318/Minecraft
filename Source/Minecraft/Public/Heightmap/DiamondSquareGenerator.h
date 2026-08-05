// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiamondSquareGenerator.generated.h"

USTRUCT(BlueprintType)
struct FDiamondSquareSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 SampleSize{ 1025 };

	UPROPERTY(BlueprintReadWrite)
	float Roughness = 0.5f;

	FDiamondSquareSettings() {}
	FDiamondSquareSettings(const int32 SampleSize) : SampleSize(SampleSize) {}
};

struct FDiamondSquareHeightmap
{
	FDiamondSquareHeightmap() {}
	
	FDiamondSquareHeightmap(const FDiamondSquareSettings& Settings)
	{
		Size = Settings.SampleSize;
		Values.Init(0.f, Settings.SampleSize * Settings.SampleSize);
	}

	int32 Size{ -1 };
	
	TArray<float> Values;
	
	void SetValue(const int32 X, const int32 Y, const float Value)
	{
		Values[GetIndex(X, Y)] = Value;
	}

	float GetValue(const int32 X, const int32 Y) const
	{
		if (IsValidIndex(X, Y))
		{
			return Values[GetIndex(X, Y)];
		}
		return 0.0f;
	}

	bool IsValid() const { return Values.Num() == Size * Size; };

	bool IsValidIndex(const int32 X, const int32 Y) const
	{
		return Values.IsValidIndex(X + Y * Size);
	}
	
private:
	int32 GetIndex(const int32 X, const int32 Y) const
	{
		return X + Y * Size;
	}
};

class DiamondSquareGenerator
{
public:
	static void Generate(const FDiamondSquareSettings& Settings, FDiamondSquareHeightmap& OutHeightmap);
	static void Normalize(FDiamondSquareHeightmap& Heightmap);
};
