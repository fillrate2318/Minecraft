#include "GameModes/VoxelGameMode.h"

#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Voxels/VoxelWorld.h"

bool AVoxelGameMode::AddVoxel_Implementation(const FVector& WorldLocation, const EVoxelType VoxelType)
{
	return VoxelWorld && VoxelWorld->AddVoxelAtWorldLocation(WorldLocation, VoxelType);
}

bool AVoxelGameMode::RemoveVoxel_Implementation(const FVector& WorldLocation)
{
	return VoxelWorld && VoxelWorld->RemoveVoxelAtWorldLocation(WorldLocation);
}

EVoxelType AVoxelGameMode::GetVoxelType_Implementation(const FVector& WorldLocation)
{
	return VoxelWorld ? VoxelWorld->GetVoxelAtWorldLocation(WorldLocation) : EVoxelType::Empty;
}

void AVoxelGameMode::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AVoxelWorld> It(GetWorld()); It; ++It)
	{
		VoxelWorld = *It;
		break;
	}

	if (!VoxelWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("VoxelGameMode could not find a VoxelWorld actor"));
		return;
	}

	if (VoxelWorld->IsWorldReady())
	{
		HandleWorldReady();
	}
	else
	{
		VoxelWorld->OnWorldReady.AddUniqueDynamic(this, &AVoxelGameMode::HandleWorldReady);
	}
}

void AVoxelGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (!IsValid(NewPlayer))
	{
		return;
	}

	if (!bWorldReady)
	{
		PendingPlayers.AddUnique(NewPlayer);
		return;
	}

	SpawnPlayer(NewPlayer);
}

void AVoxelGameMode::HandleWorldReady()
{
	if (bWorldReady)
	{
		return;
	}

	bWorldReady = true;
	VoxelWorld->OnWorldReady.RemoveDynamic(this, &AVoxelGameMode::HandleWorldReady);

	for (APlayerController* PlayerController : PendingPlayers)
	{
		if (IsValid(PlayerController))
		{
			SpawnPlayer(PlayerController);
		}
	}

	PendingPlayers.Empty();
}

void AVoxelGameMode::SpawnPlayer(APlayerController* PlayerController)
{
	FTransform SpawnTransform;
	if (!VoxelWorld || !VoxelWorld->TryGetSpawnTransform(GetPawnVerticalClearance(), SpawnTransform))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find a safe voxel-world spawn transform"));
		return;
	}

	RestartPlayerAtTransform(PlayerController, SpawnTransform);
}

float AVoxelGameMode::GetPawnVerticalClearance() const
{
	const APawn* PawnCDO = DefaultPawnClass ? DefaultPawnClass->GetDefaultObject<APawn>() : nullptr;
	const ACharacter* CharacterCDO = Cast<ACharacter>(PawnCDO);
	const UCapsuleComponent* Capsule = CharacterCDO ? CharacterCDO->GetCapsuleComponent() : nullptr;

	return Capsule ? Capsule->GetScaledCapsuleHalfHeight() + 10.0f : 100.0f;
}
