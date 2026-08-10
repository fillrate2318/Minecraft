#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/VoxelEditingInterface.h"
#include "VoxelGameMode.generated.h"

class APlayerController;
class AVoxelWorld;

UCLASS()
class MINECRAFT_API AVoxelGameMode : public AGameModeBase, public IVoxelEditingInterface
{
	GENERATED_BODY()

public:
	virtual bool AddVoxel_Implementation(const FVector& WorldLocation, EVoxelType VoxelType) override;
	virtual bool RemoveVoxel_Implementation(const FVector& WorldLocation) override;
	virtual EVoxelType GetVoxelType_Implementation(const FVector& WorldLocation) override;

protected:
	virtual void BeginPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

private:
	UFUNCTION()
	void HandleWorldReady();

	void SpawnPlayer(APlayerController* PlayerController);
	float GetPawnVerticalClearance() const;

	UPROPERTY()
	TObjectPtr<AVoxelWorld> VoxelWorld;

	UPROPERTY()
	TArray<TObjectPtr<APlayerController>> PendingPlayers;

	bool bWorldReady{ false };
};
