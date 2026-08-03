// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/WorldGeneratorSubsystem.h"
#include "Heightmap/DiamondSquareGenerator.h"

UWorldGeneratorSubsystem* UWorldGeneratorSubsystem::Get(UWorld* WorldContext)
{
	if (!WorldContext) return nullptr;

	if (const UGameInstance* GI = WorldContext->GetGameInstance())
	{
		return GI->GetSubsystem<UWorldGeneratorSubsystem>();
	}
	return nullptr;
}

void UWorldGeneratorSubsystem::GenerateHeightmap(const FDiamondSquareSettings& InSettings)
{
	TWeakObjectPtr WeakThis(this);
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [WeakThis, Settings = InSettings]()
	{
		
		FDiamondSquareHeightmap Heightmap(Settings);
		DiamondSquareGenerator::Generate(Settings, Heightmap);
		// Return to game thread to broadcast delegate
		AsyncTask(ENamedThreads::GameThread, [WeakThis, Heightmap = MoveTemp(Heightmap)]() mutable
		{
			if (UWorldGeneratorSubsystem* This = WeakThis.Get())
			{
				This->Heightmap = MoveTemp(Heightmap);
				This->OnHeightmapGenerationFinished.Broadcast();
			}
		});
	});
}

UTexture2D* UWorldGeneratorSubsystem::GetHeightmapAsTexture()
{
	const int32 Size = Heightmap.Size;
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

	UTexture2D* Texture = UTexture2D::CreateTransient(Size, Size, PF_B8G8R8A8, TEXT("DiamondSquareTexture"));
	
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
	HeightmapTexture = Texture;
	return Texture;
}
