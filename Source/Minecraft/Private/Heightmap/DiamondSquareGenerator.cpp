// Fill out your copyright notice in the Description page of Project Settings.


#include "Heightmap/DiamondSquareGenerator.h"

void UDiamondSquareGenerator::Generate(const FDiamondSquareSettings& Settings, FDiamondSquareHeightmap& OutHeightmap)
{
	int32 Seed = FMath::Rand();
	FRandomStream RandomStream(Seed);

	float Displacement = 1.f;
	const float Roughness = 0.5f;
	
	const int32 EdgeIndex = Settings.SampleSize - 1;
	
	auto GetRandomOffset = [&RandomStream, &Displacement]()
	{
		return RandomStream.FRandRange(-Displacement, Displacement);
	};

	// Fill square corners
	OutHeightmap.SetValue(0, 0, GetRandomOffset());
	OutHeightmap.SetValue(EdgeIndex, 0, GetRandomOffset());
	OutHeightmap.SetValue(0, EdgeIndex, GetRandomOffset());
	OutHeightmap.SetValue(EdgeIndex, EdgeIndex, GetRandomOffset());

	// Initial step equal to square side size
	int32 StepSize = EdgeIndex;

	// Main generator loop
	while (StepSize > 1)
	{
		const int32 HalfStepSize = StepSize / 2;
		
		// Diamond step
		for (int32 x = HalfStepSize; x < EdgeIndex; x += StepSize)
		{
			for (int32 y = HalfStepSize; y < EdgeIndex; y += StepSize)
			{
				const float A = OutHeightmap.GetValue(x - HalfStepSize, y - HalfStepSize);
				const float B = OutHeightmap.GetValue(x + HalfStepSize, y - HalfStepSize);
				const float C = OutHeightmap.GetValue(x - HalfStepSize, y + HalfStepSize);
				const float D = OutHeightmap.GetValue(x + HalfStepSize, y + HalfStepSize);
				
				const float Avg = (A + B + C + D) * 0.25f;

				OutHeightmap.SetValue(x, y, Avg + GetRandomOffset());
			}
		}

		// Square step
		for (int32 x = 0; x < EdgeIndex; x += HalfStepSize)
		{
			for (int32 y = (x + HalfStepSize) % StepSize; y < EdgeIndex; y += StepSize)
			{
				float Value = 0.f;
				int32 Count = 0;

				if (x - HalfStepSize >= 0)
				{
					Value += OutHeightmap.GetValue(x - HalfStepSize, y);
					++Count;
				}

				if (y - HalfStepSize >= 0)
				{
					Value += OutHeightmap.GetValue(x, y - HalfStepSize);
					++Count;
				}

				if (x + HalfStepSize <= EdgeIndex)
				{
					Value += OutHeightmap.GetValue(x + HalfStepSize, y);
					++Count;
				}

				if (y + HalfStepSize <= EdgeIndex)
				{
					Value += OutHeightmap.GetValue(x, y + HalfStepSize);
					++Count;
				}

				if (Count > 0)
				{
					const float Avg = Value / static_cast<float>(Count);
					OutHeightmap.SetValue(x, y, Avg + GetRandomOffset());
				}
			}
		}
		StepSize /= 2;
		Displacement *= Roughness;
	}
	Normalize(OutHeightmap);
}

void UDiamondSquareGenerator::Normalize(FDiamondSquareHeightmap& Heightmap)
{
	float MinimumValue = MAX_flt;
	float MaximumValue = -MIN_flt;

	for (const float Value : Heightmap.Values)
	{
		if (!FMath::IsFinite(Value))
		{
			continue;
		}

		MinimumValue = FMath::Min(MinimumValue, Value);

		MaximumValue = FMath::Max(MaximumValue, Value);
	}

	const float Range = MaximumValue - MinimumValue;

	if (FMath::IsNearlyZero(Range))
	{
		Heightmap.Values.Init(0.0f, Heightmap.Values.Num());
		return;
	}

	const float InverseRange = 1.0f / Range;

	for (float& Value : Heightmap.Values)
	{
		Value = (Value - MinimumValue) * InverseRange;
	}
}
