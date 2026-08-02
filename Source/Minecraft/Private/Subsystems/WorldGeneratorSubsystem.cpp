// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/WorldGeneratorSubsystem.h"

#include "Heightmap/DiamondSquareGenerator.h"

UTexture2D* UWorldGeneratorSubsystem::GenerateHeightmapTexture()
{
	FDiamondSquareSettings Settings(1025);
	FDiamondSquareHeightmap Heightmap(Settings);
	
	UDiamondSquareGenerator::Generate(Settings, Heightmap);
	
	const int32 Size = 1025;
	const int32 PixelCount = Size * Size;
	
	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(PixelCount);

	for (int32 Y = 0; Y < Size; ++Y)
	{
		for (int32 X = 0; X < Size; ++X)
		{
			const int32 Index = Y * Size + X;

			const float HeightAlpha = FMath::Clamp(Heightmap.GetValue(X, Y), 0.0f, 1.0f);

			const uint8 GrayValue = static_cast<uint8>(FMath::RoundToInt(HeightAlpha * 255.0f));

			Pixels[Index] = FColor(GrayValue, GrayValue, GrayValue, 255);
		}
	}

	UTexture2D* Texture = UTexture2D::CreateTransient(Size, Size, PF_B8G8R8A8, TEXT("DiamondSquareDebugTexture"));
	
	Texture->SRGB = false;
	Texture->Filter = TF_Nearest;
	Texture->AddressX = TA_Clamp;
	Texture->AddressY = TA_Clamp;
	Texture->NeverStream = true;

	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];

	void* TextureData = Mip.BulkData.Lock(LOCK_READ_WRITE);

	FMemory::Memcpy(TextureData, Pixels.GetData(), Pixels.Num() * sizeof(FColor));

	Mip.BulkData.Unlock();
	Texture->UpdateResource();
	HeightmapDebugTexture = Texture;
	return Texture;
}
