#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VoxelGameMode.generated.h"

class APlayerController;
class AVoxelWorld;

UCLASS()
class MINECRAFT_API AVoxelGameMode : public AGameModeBase
{
	GENERATED_BODY()

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
